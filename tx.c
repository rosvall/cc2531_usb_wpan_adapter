// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "bsp/csp.h"
#include "bsp/radio.h"
#include "bsp/usb.h"
#include "bsp/mac_timer.h"

#include "usb_config.h"

#include "log.h"

#include "tx.h"


// Settings
static u8 csma_be_min;
static u8 csma_be_max;
static u8 csma_retries;

// CSMA BACK-OFF PERIOD:
// aUnitBackoffPeriod = aTurnaroundTime + aCcaTime.
// aTurnaroundTime = 1ms (1 ms expressed in symbol periods, rounded up to the next integer number of symbol periods)
// aCcaTime = 8 symbol periods

inline void
setup_mac_timer(void)
{
	// NOTE: Apparently mac timer is fed from 32MHz clock undivided!
	// Symbol rate: 62.5k/s => symbol period: 16us
	// 32MHz / 62.5Khz = 512 ticks
	const u16 symbol_period = 32000000/62500;
	const u16 backoff_period = (8 + 63)*symbol_period;
	mac_timer_set_period(backoff_period);

	// Don't reset overflow count (use long overflow period)
	mac_timer_select_multiplexed_regs(T2M_TIMER, T2OVF_PERIOD);
	T2MOVF0 = 0xff;
	T2MOVF1 = 0xff;
	T2MOVF2 = 0xff;

	// Set overflow compare 1 count to something like 100 ms => 100 overflows
	mac_timer_select_multiplexed_regs(T2M_TIMER, T2OVF_CMP1);
	T2MOVF0 = 100;
	T2MOVF1 = 0;
	T2MOVF2 = 0;
}

inline void
reset_and_start_mac_timer(void)
{
	// Stop mac timer
	T2CTRL = 0;

	// Clear count and overflow count
	mac_timer_select_multiplexed_regs(T2M_TIMER, T2OVF_OVERFLOW);
	// clear count
	T2M0 = 0;
	T2M1 = 0;

	// clear overflow count
	T2MOVF0 = 0;
	T2MOVF1 = 0;
	T2MOVF2 = 0;

	// Start mac timer
	T2CTRL = T2CTRL_RUN;
}

static void
write_csp_csma_program(void)
{
	LOGD(__func__);

	// Clear everything
	RFST = CSP_IMM_CMD_STROBE(CSP_CMD_STOP);
	RFST = CSP_IMM_CMD_STROBE(CSP_CMD_CLEAR);

	// Reg X: back-off time in units of MAC timer periods
	// Reg Y: back-off exponent
	// Reg Z: back-off retries

	RFST = CSP_INSN_INCZ;

	// do {
	RFST = CSP_INSN_LABEL;

		// if (Y != 0) {
		RFST = CSP_INSN_SKIP(CSP_IF_Y_0, 2);
			// RANDXY won't do Y=0

			// X = random() & ((1<<Y)-1)
			RFST = CSP_INSN_RANDXY;
			// sleep(X)
			RFST = CSP_INSN_WAITX;
		// }

		// Y = min(Y+1, max_be)
		RFST = CSP_INSN_INCMAXY(csma_be_max);

		RFST = CSP_INSN_SKIP(CSP_IF_SFD, 3);

		// if (CCA) {
		RFST = CSP_INSN_SKIP(CSP_IF_NOT_CCA, 2);
			// Transmit
			RFST = CSP_INSN_STROBE(CSP_CMD_TXON);
			// Stop
			RFST = CSP_INSN_STROBE(CSP_CMD_STOP);
		// }

		// --Z
		RFST = CSP_INSN_DECZ;

	// } while (Z != 0)
	RFST = CSP_INSN_RPT(CSP_IF_Z_NOT_0);

	// We didn't send :( Issue MANINT intr
	RFST = CSP_INSN_INT;
}

void
tx_csma(void)
{
	LOGD(__func__);

	RADIO.csp.x = 0;
	RADIO.csp.y = csma_be_min;
	RADIO.csp.z = csma_retries;

	RFST = CSP_IMM_CMD_STROBE(CSP_CMD_START);

	reset_and_start_mac_timer();
}

void
tx_now(void)
{
	LOGI(__func__);
	RFST = CSP_IMM_CMD_STROBE(CSP_CMD_TXON);
}

__bit
tx_prepare(u8 msdu_len)
{
	LOGDX8(__func__, msdu_len);

	RFST = CSP_IMM_CMD_STROBE(CSP_CMD_FLUSHTX);
	RFD = msdu_len + 2;

	return 0;
}

void
tx_set_csma_params(u16 packed_params)
{
	u8 l = packed_params;
	u8 h = packed_params>>8;

	csma_be_min = l&0x7;
	csma_be_max = (l>>4)&0x7;
	csma_retries = h;

	LOGDX8("be_min", csma_be_min);
	LOGDX8("be_max", csma_be_max);
	LOGDX8("retries", csma_retries);

	write_csp_csma_program();
}

inline void
setup_txstatus_endpoint(void)
{
	usb_select_endpoint(INT_EP);

	USB.in_ep.csih = USBCSIH_IN_DBL_BUF | USBCSIH_ENABLE;
	USB.in_ep.maxi = INT_EP_MAXPKTSIZE / 8;

	USB.in_ep.csil = USBCSIL_CLR_DATA_TOG;
	USB.in_ep.csil = USBCSIL_FLUSH_PACKET;
	USB.in_ep.csil = USBCSIL_FLUSH_PACKET;

	USB.iie |= BIT(INT_EP);
}

inline void
setup_radio_tx(void)
{
	RFST = CSP_IMM_CMD_STROBE(CSP_CMD_FLUSHTX);

	// Clear intr flags
	RFIRQF1 = 0;

	RADIO.rfirqm1 = RFIRQF1_TXACKDONE | RFIRQF1_TXDONE | RFIRQF1_CSP_MANINT;
}

void
tx_setup(void)
{
	setup_txstatus_endpoint();
	setup_radio_tx();
	setup_mac_timer();
}

void
usb_status_send(u8 status)
{
	USB.fifo[INT_EP].fifo = status;
	usb_select_endpoint(INT_EP);
	USB.in_ep.csil = USBCSIL_INPKT_RDY;

	LOGDX8("tx status", status);
}

void
tx_radio_intr_handler(u8 flags)
{
	if (flags & RFIRQF1_CSP_MANINT) {
		usb_status_send(IEEE802154_CHANNEL_ACCESS_FAILURE);
	}

	if (flags & RFIRQF1_TXDONE) {
		usb_status_send(IEEE802154_SUCCESS);
	}

	if (flags & RFIRQF1_TXACKDONE) {
		LOGD("autoack");
	}
}

void
tx_usb_intr_handler(void)
{
	usb_select_endpoint(INT_EP);

	u8 flags = USB.in_ep.csil;

	if (flags & USBCSIL_SENT_STALL) {
		LOGE("ST EP: STALLED");
	}
	
	USB.in_ep.csil = 0;
}
