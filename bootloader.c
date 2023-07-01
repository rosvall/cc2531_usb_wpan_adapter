// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "bsp/interrupts.h"
#include "log.h"

inline void
hang()
{
	for (;;)
		;
}

void
bootloader_enter()
{
	interrupts_disable();
	LOGI("Resetting to bootloader through watchdog timeout");
	hang();
}
