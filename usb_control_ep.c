// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "bsp/dma.h"
#include "bsp/mem.h"
#include "bsp/radio.h"
#include "bsp/usb.h"

#include "usb/descriptor.h"

#include "int.h"
#include "log.h"
#include "rx.h"
#include "tx.h"
#include "bootloader.h"
#include "usb_config.h"
#include "dyn_usb_desc.h"
#include "const_usb_desc.h"

#define DMA_CH 0

#define NOT_FIFO 1
#define IS_FIFO  0

static struct dma_conf dma;
static void (* request_done)(void);
static struct usb_setup request;
static enum {
	STATE_IDLE,
	STATE_RX,
	STATE_TX,
	STATE_DONE,
	STATE_STALL,
} state;
static __xdata u8 current_configuration;

#define SET_STATE(_st)                                                         \
	{                                                                          \
		state = _st;                                                           \
	}

static void
copy_chunk_with_dma(void)
{
	u8 n = CTRL_EP_MAXPKTSIZE;
	do {
		if (!dma_is_armed(DMA_CH)) {
			SET_STATE(STATE_DONE);
			return;
		}

		dma_trig(DMA_CH);
	} while (--n);
}

static void
setup_tx_dma(const void __xdata * src, __bit not_fifo)
{
	LOGDX16(__func__, (u16)src);

	dma_set_src(dma, src);
	dma_set_dst(dma, &USB.fifo[CTRL_EP].fifo);
	dma_set_len(dma, request.wLength);
	u8 mode2 = DMA_MODE2(PRIORITY_HIGH, NO_MASK8, INTR_DISABLE, DST_CONST, SRC_CONST);
	mode2 |= not_fifo << DMA_MODE2_SRCMODE_SHIFT;
	dma.mode2 = mode2;

	dma_arm(DMA_CH);
	SET_STATE(STATE_TX);
}

static void
setup_rx_dma(void __xdata * dst, __bit not_fifo)
{
	LOGDX16(__func__, (u16)dst);

	dma_set_src(dma, &USB.fifo[CTRL_EP].fifo);
	dma_set_dst(dma, dst);
	dma_set_len(dma, request.wLength);
	u8 mode2 = DMA_MODE2(PRIORITY_HIGH, NO_MASK8, INTR_DISABLE, DST_CONST, SRC_CONST);
	mode2 |= not_fifo << DMA_MODE2_DSTMODE_SHIFT;
	dma.mode2 = mode2;

	dma_arm(DMA_CH);
	SET_STATE(STATE_RX);
}

static __bit
invalid_ep(void)
{
	u8 ep = request.wIndex;

	if (USB_EP_INDEX(ep) > 5)
		return 1;

	usb_select_endpoint(ep);

	u8 configured_ep_size = USB_EP_IS_IN(ep) ? USB.in_ep.maxi : USB.out_ep.maxo;

	usb_select_endpoint(CTRL_EP);

	return !configured_ep_size;
}

inline void
halt_ep(void)
{
	// Halt (stall) EP

	LOGDX8(__func__, request.wIndex);

	u8 ep = request.wIndex;

	usb_select_endpoint(ep);

	if (USB_EP_IS_IN(ep))
		USB.in_ep.csil |= USBCSIL_SEND_STALL;
	else
		USB.out_ep.csol |= USBCSOL_SEND_STALL;

	usb_select_endpoint(CTRL_EP);

	SET_STATE(STATE_DONE);
}

inline void
resume_ep(void)
{
	// Un-halt EP (clear stall and reset data toggle)

	LOGDX8(__func__, request.wIndex);

	u8 ep = request.wIndex;

	usb_select_endpoint(ep);

	if (USB_EP_IS_IN(ep))
		USB.in_ep.csil = USBCSIL_CLR_DATA_TOG | USBCSIL_FLUSH_PACKET;
	else
		USB.out_ep.csol = USBCSOL_CLR_DATA_TOG | USBCSOL_FLUSH_PACKET;
	
	usb_select_endpoint(CTRL_EP);
}

inline __bit
is_ep_halted(void)
{
	u8 ep = request.wIndex;

	usb_select_endpoint(ep);

	u8 is_halted;

	if (USB_EP_IS_IN(ep))
		is_halted = USB.in_ep.csil & USBCSIL_SEND_STALL;
	else
		is_halted = USB.out_ep.csol & USBCSOL_SEND_STALL;

	usb_select_endpoint(CTRL_EP);

	return is_halted;
}

static void
get_ep_status(void)
{
	// Send 0x0001 if endpoint is stalled, 0x0000 otherwise.

	LOGDX8(__func__, request.wIndex);

	if (invalid_ep() || request.wLength != 2) {
		SET_STATE(STATE_STALL);
		return;
	}

	static __xdata u16 status;

	status = is_ep_halted();

	setup_tx_dma(&status, NOT_FIFO);
}

static void
set_ep_feature(void)
{
	LOGDX8(__func__, request.wValue);

	if (invalid_ep() || request.wLength != 0 || request.wValue != USB_FEATURE_ENDPOINT_HALT) {
		SET_STATE(STATE_STALL);
		return;
	}

	halt_ep();
	SET_STATE(STATE_DONE);
}

static void
clear_ep_feature(void)
{
	LOGDX8(__func__, request.wValue);

	if (invalid_ep() || request.wLength != 0 || request.wValue != USB_FEATURE_ENDPOINT_HALT) {
		SET_STATE(STATE_STALL);
		return;
	}

	resume_ep();
	SET_STATE(STATE_DONE);
}

static void
send_zeroes(u8 n)
{
	if (request.wLength != n) {
		SET_STATE(STATE_STALL);
		return;
	}
	setup_tx_dma(&USB._reserved_zeroes, IS_FIFO);
}

static void
get_dev_status(void)
{
	LOGD(__func__);
	// Send 16 bits.
	// bit 0: self powered
	// bit 1: remote wakeup
	// rest: 0/reserved

	// We're never self powered, and don't support remote wakeup.
	send_zeroes(2);
}

static void
get_iface_status(void)
{
	LOGD(__func__);
	// iface = request.wIndex
	// Send 16 bits.
	// all bits: 0/reserved
	send_zeroes(2);
}

static void
set_configuration(void)
{
	u8 conf = request.wValue;

	LOGDX8(__func__, conf);

	current_configuration = conf;

	if (conf) {
		rx_setup();
		tx_setup();
		usb_select_endpoint(CTRL_EP);
	}

	SET_STATE(STATE_DONE);
}

static void
get_configuration(void)
{
	LOGD(__func__);

	if (request.wLength != 1) {
		SET_STATE(STATE_STALL);
		return;
	}

	setup_tx_dma(&current_configuration, IS_FIFO);
}

static void
set_address(void)
{
	u8 addr = request.wValue;

	LOGDX8(__func__, addr);

	USB.addr = addr;

	current_configuration = 0;
	SET_STATE(STATE_DONE);
}

static void
get_descriptor(void)
{
	LOGDX16(__func__, request.wValue);

	const __xdata void * desc;
	const __code void * const_desc = const_usb_desc_get(request.wValue);
	if (const_desc)
		desc = mmap_code_to_xdata(const_desc);
	else
		desc = dyn_usb_desc_get(request.wValue);

	if (!desc) {
		SET_STATE(STATE_STALL);
		return;
	}

	u8 len = usb_desc_total_len(desc);
	if (len < request.wLength)
		request.wLength = len;

	setup_tx_dma(desc, NOT_FIFO);
}

static void
get_iface_altsetting(void)
{
	LOGD(__func__);
	// iface = request.wIndex
	// Send one byte: alternate setting value

	// We're only have alt setting 0
	send_zeroes(1);
}

static void
set_iface_altsetting(void)
{
	LOGD(__func__);

	if (request.wLength != 0) {
		SET_STATE(STATE_STALL);
		return;
	}

	u8 alt  = request.wValue;
	u8 intf = request.wIndex;

	if (intf >= USB_INTERFACE_COUNT) {
		SET_STATE(STATE_STALL);
		return;
	}

	// We're only have alt setting 0

	if (alt != 0) {
		SET_STATE(STATE_STALL);
		return;
	}

	SET_STATE(STATE_DONE);
}

static void
vendor_xdata_write(void)
{
	LOGDX16(__func__, request.wValue);
	setup_rx_dma((u8 __xdata *)request.wValue, NOT_FIFO);
}

static void
vendor_xdata_read(void)
{
	LOGDX16(__func__, request.wValue);
	setup_tx_dma((u8 __xdata *)request.wValue, NOT_FIFO);
}

static void
vendor_fifo_write(void)
{
	LOGDX16(__func__, request.wValue);
	setup_rx_dma((u8 __xdata *)request.wValue, IS_FIFO);
}

static void
vendor_fifo_read(void)
{
	LOGDX16(__func__, request.wValue);
	setup_tx_dma((u8 __xdata *)request.wValue, IS_FIFO);
}

static void
vendor_tx(void)
{
	__bit err = tx_prepare(request.wLength);
	if (err) {
		SET_STATE(STATE_STALL);
	} else {
		// Write usb control packet data directly to TXFIFO
		setup_rx_dma(&X_RFD, IS_FIFO);
		if (request.wValue)
			request_done = tx_now;
		else
			request_done = tx_csma;
	}
}

static void
vendor_set_csma(void)
{
	tx_set_csma_params(request.wValue);
	SET_STATE(STATE_DONE);
}

static void
dfu_detach(void)
{
	SET_STATE(STATE_DONE);
	request_done = bootloader_enter;
}

static void
handle_request(void)
{
	u8 rt = request.bmRequestType;
	u8 req = request.bRequest;

#undef RT
#undef REQ

#define RT(_name, _expr)                                                       \
	if (rt == USB_RT_##_name) {                                                \
		_expr                                                                  \
	}

#define REQ(_name, _fn)                                                        \
	if (req == USB_REQ_##_name)                                                \
	{                                                                          \
		_fn();                                                                 \
		return;                                                                \
	}

	RT(VENDOR_DEV_OUT,
		REQ(VENDOR_XDATA_WRITE, vendor_xdata_write) 
		REQ(VENDOR_FIFO_WRITE,  vendor_fifo_write)
		REQ(VENDOR_TX,          vendor_tx) 
		REQ(VENDOR_SET_CSMA,    vendor_set_csma)
	)
	RT(VENDOR_DEV_IN, 
		REQ(VENDOR_XDATA_READ,  vendor_xdata_read)
		REQ(VENDOR_FIFO_READ,   vendor_fifo_read)
	)
	RT(STD_DEV_OUT, 
		REQ(SET_ADDRESS,        set_address) 
		REQ(SET_CONFIGURATION,  set_configuration)
	)
	RT(STD_DEV_IN, 
		REQ(GET_STATUS,         get_dev_status) 
		REQ(GET_DESCRIPTOR,     get_descriptor)
		REQ(GET_CONFIGURATION,  get_configuration)
	)
	RT(STD_EP_OUT, 
		REQ(SET_FEATURE,        set_ep_feature) 
		REQ(CLEAR_FEATURE,      clear_ep_feature)
	)
	RT(STD_EP_IN, 
		REQ(GET_STATUS,         get_ep_status)
	)
	RT(STD_INTF_OUT, 
		REQ(SET_INTF,           set_iface_altsetting)
	)
	RT(STD_INTF_IN, 
		REQ(GET_STATUS,         get_iface_status) 
		REQ(GET_INTF,           get_iface_altsetting)
	)
	RT(CLASS_INTF_OUT, 
		REQ(DFU_DETACH,         dfu_detach)
	)

	SET_STATE(STATE_STALL);
}

static void
recv_request(void)
{
	__data u8 * __data dst = (__data u8 * __data) & request;
	u8 len = sizeof(request);
	do {
		*dst++ = USB.fifo[CTRL_EP].fifo;
	} while (--len);
}

static void
do_nothing(void)
{
}

void
usb_control_intr_handler(void)
{
	usb_select_endpoint(CTRL_EP);

	u8 flags = USB.ctrl_ep.cs0;

	if (flags & USBCS0_SETUP_END) {
		USB.ctrl_ep.cs0 = USBCS0_CLR_SETUP_END;
		SET_STATE(STATE_IDLE);
		LOGW("EP0: setup end");
	}

	if (flags & USBCS0_SENT_STALL) {
		USB.ctrl_ep.cs0 = 0;
		SET_STATE(STATE_IDLE);
		LOGW("EP0: sent stall");
		LOGWX16("request", *(u16 *)&request);
		LOGWX16("wValue ", request.wValue);
		LOGWX16("wIndex ", request.wIndex);
		LOGWX16("wLength", request.wLength);
	}

	if (flags & USBCS0_OUTPKT_RDY) {
		if (state == STATE_IDLE) {
			request_done = do_nothing;
			recv_request();
			handle_request();
		} else if (state == STATE_RX) {
			copy_chunk_with_dma();

			if (!dma_is_armed(DMA_CH))
				SET_STATE(STATE_DONE);
		}

		u8 reg = USBCS0_CLR_OUTPKT_RDY;
		if (state == STATE_STALL)
			reg |= USBCS0_SEND_STALL;
		else if (state == STATE_DONE)
			reg |= USBCS0_DATA_END;
		USB.ctrl_ep.cs0 = reg;
	}

	if (state == STATE_TX) {
		copy_chunk_with_dma();
		u8 reg = USBCS0_INPKT_RDY;
		if (state == STATE_DONE)
			reg |= USBCS0_DATA_END;
		USB.ctrl_ep.cs0 = reg;
	}

	if (state == STATE_DONE) {
		SET_STATE(STATE_IDLE);
		request_done();
	}
}

void
usb_control_reset(void)
{
	current_configuration = 0;
	SET_STATE(STATE_IDLE);
}

void
usb_control_init(void)
{
	dyn_usb_desc_init();
	dma_set_mode1(dma, TRIG_NONE, BYTEMODE, ONESHOT, WORD8);
	dma_init_ch0(mmap_idata_to_xdata(&dma));
	usb_control_reset();
}
