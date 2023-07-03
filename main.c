// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "led.h"
#include "sleep.h"
#include "bsp/clock.h"
#include "bsp/flash.h"
#include "bsp/interrupts.h"
#include "bsp/radio.h"
#include "bsp/watchdog.h"
#include "config/pins.h"
#include "int.h"
#include "log.h"
#include "radio.h"
#include "uart.h"
#include "usb.h"


inline void
pins_setup()
{
	gpio_dir_out(LED_GREEN);
	gpio_dir_out(LED_RED);
}

inline void
print_csp_state()
{
	u8 state;
	state = RADIO.fsmstat0.fsm_ffctrl_state;

	led_red_off();
	led_green_off();

	if (state > 0x1f) {
		led_green_on();
	} else if (state > 0x6) {
		led_red_on();
	}
}

int
main()
{
	FLASH.ctl = FLASH_CTL_CACHE_MODE_PREFETCH;
	clk_setup(CLKSPD_32M, TICKSPD_32M, OSC_32MHZ_XTAL, OSC32K_RC);
	pins_setup();
	uart_setup();

	LOGI("CC2531 WPAN adapter " GIT_VERSION_STR " online!");

	pins_setup();
	usb_init();
	radio_setup();

	interrupts_enable();

	for (;;) {
		watchdog_feed();
		maybe_sleep();
		print_csp_state();
	}
}
