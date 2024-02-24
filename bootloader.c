// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "bsp/interrupts.h"
#include "log.h"

inline void
hang(void)
{
	for (;;)
		;
}

void
bootloader_enter(void)
{
	interrupts_disable();
	LOGI("Resetting to bootloader through watchdog timeout");
	hang();
}
