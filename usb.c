// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "bsp/gpio.h"
#include "bsp/interrupts.h"
#include "bsp/usb.h"
#include "config/pins.h"
#include "int.h"
#include "log.h"
#include "radio.h"
#include "rx.h"
#include "sleep.h"
#include "tx.h"
#include "usb.h"
#include "usb_config.h"
#include "usb_control_ep.h"

extern __bit sleep_now;

static void
enable_usb_pll()
{
	LOGI(__func__);

	USB.ctrl = USBCTRL_USB_EN | USBCTRL_PLL_EN;
	while (!(USB.ctrl & USBCTRL_PLL_LOCKED))
		;
}

static void
disable_usb_pll()
{
	LOGI(__func__);

	USB.ctrl = USBCTRL_USB_EN;
	while (USB.ctrl & USBCTRL_PLL_LOCKED)
		;
}

inline void
usb_reset()
{
	LOGI(__func__);

	radio_stop();

	USB.cie = USBCI_RST | USBCI_SUSPEND;
	USB.pow = USBPOW_SUSPEND_EN;

	usb_control_reset();
}

inline void
usb_suspend()
{
	LOGI(__func__);
	disable_usb_pll();
	sleep_soon();
}

inline void
usb_resume()
{
	LOGI(__func__);
	enable_usb_pll();
}

inline void
check_wakeup_flag()
{
	if (P2IFG & P2IFG_DPIF)
		usb_resume();

	P2IFG = 0;
}

inline void
check_common_flags()
{
	// Cleared on read
	u8 flags = USB.cif;
	if (flags & USBCI_RST)
		usb_reset();

	if (flags & USBCI_SUSPEND)
		usb_suspend();
}

inline void
check_in_ep_flags()
{
	// Cleared on read
	u8 flags = USB.iif;

	if (flags & BIT(CTRL_EP))
		usb_control_intr_handler();

	if (flags & BIT(RXPKT_EP))
		rx_usb_intr_handler();

	if (flags & BIT(INT_EP))
		tx_usb_intr_handler();
}

inline void
check_out_ep_flags()
{
	// Cleared on read
	u8 flags = USB.oif;
	if (flags)
		LOGWX8("oif", flags);
}

INTERRUPT(usb_intr_handler, INTR_P2INT_USB_I2C)
{
	IRCON2_P2IF = 0;
	check_common_flags();
	check_in_ep_flags();
	check_out_ep_flags();
	check_wakeup_flag();
}

void
usb_init()
{
	enable_usb_pll();

	gpio_dir_out(USB_DPLUS);
	gpio_set_high(USB_DPLUS);

	// Clear interrupt flags
	USB.iif = 0;
	USB.cif = 0;
	USB.oif = 0;
	P2IFG = 0;
	IRCON2_P2IF = 0;

	// Enable D+ intr on wake up
	P2IEN |= P2IEN_DPIEN;

	// Enable P2 interrupt
	IEN2 |= IEN2_P2IE;

	usb_control_init();
}
