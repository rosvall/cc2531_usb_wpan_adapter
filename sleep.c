// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "bsp/clock.h"
#include "bsp/interrupts.h"
#include "bsp/sleep.h"
#include "bsp/watchdog.h"

#include "led.h"
#include "log.h"
#include "radio.h"

#include "sleep.h"

__bit sleep_flag;

static void
sleep()
{
	// The instruction immediately following the write to PCON 
	// is only correctly executed on resume if NOT 4-byte aligned!

	// NOTE: Flash cache MUST be on!

	// Align the 3 byte 'mov PCON, #PCON_IDLE' to an even address, so
	// the next instructon is on an odd address
	__asm__(".even");
	PCON = PCON_IDLE;
	interrupts_disable();

	// And not we resume here and return to caller, 
	// instead of the interrupt handler that woke us up...
}

void
sleep_enter_pm1()
{
	LOGI(__func__);

	radio_stop();

	watchdog_feed();

	led_red_off();
	led_green_off();

	// Go to sleep
	SLEEPCMD = SLEEPCMD_MODE_PM1;
	sleep();

	// We're awake again with interrupts off

	// Wait for oscillator to be stable again
	do {
		watchdog_feed();
	} while (CLKCONSTA & CLK_OSC_16MHZ_RC);

	LOGI("resuming now");

	// Now let's handle the interrupt that woke us up
	interrupts_enable();
}
