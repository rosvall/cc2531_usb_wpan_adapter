// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "int.h"
#include "bsp/radio.h"
#include "bsp/csp.h"
#include "bsp/obs.h"
#include "bsp/interrupts.h"
#include "bsp/infopage.h"
#include "bsp/gpio.h"

#include "config/pins.h"
#include "log.h"
#include "rx.h"
#include "tx.h"

void
radio_setup(void)
{
	LOGD(__func__);
	
	// Set up packet sniffing outputs
	RADIO.mdmtest1.rfc_sniff_en = 1;

	obs_enable(OBS_INTR_PIN,      40);
	obs_enable(OBS_TX_ACTIVE_PIN, RFC_OBS_SIG0);
	obs_enable(OBS_DATA_PIN,      RFC_OBS_SIG1);
	obs_enable(OBS_CLK_PIN,       RFC_OBS_SIG2);

	RADIO.rfc_obs_ctrl[0] = RFC_OBS_MUX_TX_ACTIVE;
	RADIO.rfc_obs_ctrl[1] = RFC_OBS_MUX_RFC_SNIFF_DATA;
	RADIO.rfc_obs_ctrl[2] = RFC_OBS_MUX_RFC_SNIFF_CLK;
	
	// ref: Table 23-6. Registers That Require Update From Their Default Value
	RADIO.agcctrl1_agc_ref = 0x15;
	RADIO.txfiltcfg_fc = 0x9;
	RADIO.fscal1_vco_curr = 0;

	// Use sane defaults
	RADIO.cca_thr = -8;
	// RADIO.txpower = 0x45;

	RADIO.rferrm = RFERRF_NLOCK | RFERRF_RXABO | RFERRF_RXOVERF | RFERRF_RXUNDERF | RFERRF_TXOVERF | RFERRF_TXUNDERF | RFERRF_STROBEERR;

	// Enable auto ack
	RADIO.frmctrl0.autoack = 1;

	// RADIO.srcmatch.src_match_en = 0;

	// copy permanent ieee addr from info page to frame filter reg
	RADIO.ext_add = INFOPAGE.ieee_addr;

	// RADIO.fsmctrl.rx2rx_time_off = 0;

	// Clear intr flags
	RFERRF = 0;
	S1CON = 0;

	// Enable RF intr
	IEN2 |= IEN2_RFIE;
	// Enable RFERR intr
	IEN0_RFERRIE = 1;
}

INTERRUPT(rferr_isr, INTR_RFERR)
{
	TCON_RFERRIF = 0;

	u8 flags = RFERRF;
	RFERRF = 0;

	if (flags & RFERRF_NLOCK) {
		// Frequency synthesizer failed to achieve lock after time-out, or lock is lost during reception
		// Receiver must be restarted to clear this error situation.
		RFST = CSP_IMM_CMD_STROBE(CSP_CMD_RXON);
		LOGE("nlock");
	}
	
	if (flags & RFERRF_RXABO) {
		RFST = CSP_IMM_CMD_STROBE(CSP_CMD_FLUSHRX);
		LOGE("rxabo");
	}
	
	if (flags & RFERRF_RXOVERF) {
		RFST = CSP_IMM_CMD_STROBE(CSP_CMD_FLUSHRX);
		RFST = CSP_IMM_CMD_STROBE(CSP_CMD_RXON);
		LOGE("rx overflow");
	}
	
	if (flags & RFERRF_RXUNDERF) {
		RFST = CSP_IMM_CMD_STROBE(CSP_CMD_FLUSHRX);
		RFST = CSP_IMM_CMD_STROBE(CSP_CMD_RXON);
		LOGE("rx underflow");
	}
	
	if (flags & RFERRF_TXOVERF) {
		RFST = CSP_IMM_CMD_STROBE(CSP_CMD_FLUSHTX);
		usb_status_send(IEEE802154_TRANSACTION_OVERFLOW);
		LOGE("tx overflow");
	}
	
	if (flags & RFERRF_TXUNDERF) {
		RFST = CSP_IMM_CMD_STROBE(CSP_CMD_FLUSHTX);
		usb_status_send(IEEE802154_SYSTEM_ERROR);
		LOGE("tx underflow");
	}
	
	if (flags & RFERRF_STROBEERR) {
		// A command strobe was issued at a time it could not be processed.
		// Triggered if trying to disable radio when already disabled,
		// or when trying to do a SACK, SACKPEND, or SNACK command when not in active RX.
		LOGE("strobe err");
	}
}

INTERRUPT(rf_isr, INTR_RF)
{
	// clear interrupt flags
	S1CON = 0;

	u8 masked_flags;

	masked_flags = RFIRQF1 & RADIO.rfirqm1;
	if (masked_flags) {
		RFIRQF1 = 0;
		tx_radio_intr_handler(masked_flags);
	}

	masked_flags = RFIRQF0 & RADIO.rfirqm0;
	if (masked_flags) {
		RFIRQF0 = 0;
		rx_radio_intr_handler(masked_flags);
	}
}

void
radio_stop(void)
{
	LOGD(__func__);

	if (RADIO.fsmstat0.fsm_ffctrl_state)
		RFST = CSP_IMM_CMD_STROBE(CSP_CMD_RFOFF);

	RFST = CSP_IMM_CMD_STROBE(CSP_CMD_FLUSHRX);
}
