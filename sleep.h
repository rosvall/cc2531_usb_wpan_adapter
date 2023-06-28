// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

extern __bit sleep_flag;

void
sleep_enter_pm1();

// Call this from interrupt context to schedule sleep
inline void
sleep_soon()
{
	sleep_flag = 1;
}

// Call this periodically to sleep if needed from normal context
inline void
maybe_sleep()
{
	if (sleep_flag) {
		sleep_flag = 0;
		sleep_enter_pm1();
	}
}
