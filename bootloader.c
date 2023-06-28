// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "bsp/gpio.h"
#include "bsp/interrupts.h"
#include "config/pins.h"
#include "log.h"
#include "bootloader.h"

inline void
hang()
{
	for (;;) {
		__asm__ ("nop");
	}
}

void
bootloader_enter()
{
	interrupts_disable();
	LOGI("Resetting to bootloader through watchdog timeout");
	hang();
}
