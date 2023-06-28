// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "bsp/csp.h"
#include "bsp/usb.h"
#include "bsp/radio.h"

#include "int.h"
#include "log.h"
#include "usb_config.h"

#define enable_radio_pkt_ready_intr()  { RADIO.rfirqm0 = RFIRQF0_FIFOP; }
#define disable_radio_pkt_ready_intr() { RADIO.rfirqm0 = 0; }

inline void
setup_radio_rx()
{
	RFST = CSP_IMM_CMD_STROBE(CSP_CMD_FLUSHRX);
	RFST = CSP_IMM_CMD_STROBE(CSP_CMD_FLUSHRX);
	
	// Clear intr flags
	RFIRQF0 = 0;

	// We only want an interrupt when a complete frame has been received
	RADIO.fifop_thr = 127;
}

inline void
setup_usb_rx_endpoint()
{
	usb_select_endpoint(RXPKT_EP);

	USB.in_ep.maxi = RXPKT_EP_MAXPKTSIZE / 8;

	USB.in_ep.csil = USBCSIL_CLR_DATA_TOG;
	USB.in_ep.csil = USBCSIL_FLUSH_PACKET;
	USB.in_ep.csil = USBCSIL_FLUSH_PACKET;

	USB.in_ep.csih = USBCSIH_AUTOSET | USBCSIH_IN_DBL_BUF | USBCSIH_ENABLE;

	USB.iie |= BIT(RXPKT_EP);
}

void
rx_setup()
{
	LOGI(__func__);
	setup_usb_rx_endpoint();
	setup_radio_rx();

	enable_radio_pkt_ready_intr();
}

inline void
rx_pkt()
{
	LOGI("rx pkt");

	// pop phy header (frame length) from fifo
	u8 len = RFD & 0x7f;
	do {
		USB.fifo[RXPKT_EP].fifo = RFD;
	} while (--len);

	// Tell usb hw there's a packet to send
	usb_select_endpoint(RXPKT_EP);
	USB.in_ep.csil = USBCSIL_INPKT_RDY;
}

void
rx_radio_intr_handler(u8 flags)
{
	if (flags & RFIRQF0_FIFOP) {
		// A complete frame has been received
		rx_pkt();
		// Disable this interrupt until usb fifo is empty
		disable_radio_pkt_ready_intr();
	}
}

void
rx_usb_intr_handler()
{
	usb_select_endpoint(RXPKT_EP);

	u8 flags = USB.in_ep.csil;

	if (flags & USBCSIL_SENT_STALL) {
		USB.in_ep.csil = 0;
		LOGE("rx ep: stalled");
	} else if (!flags) {
		// Last pkt in usb fifo has been sent to host
		enable_radio_pkt_ready_intr();
	}
}
