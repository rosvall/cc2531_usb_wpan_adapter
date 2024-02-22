// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "bsp/gpio.h"
#include "config/pins.h"

inline void
led_green_on(void)
{
	gpio_set_low(LED_GREEN);
}

inline void
led_green_off(void)
{
	gpio_set_high(LED_GREEN);
}

inline void
led_red_on(void)
{
	gpio_set_high(LED_RED);
}

inline void
led_red_off(void)
{
	gpio_set_low(LED_RED);
}
