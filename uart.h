// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "bsp/gpio.h"
#include "bsp/usart.h"
#include "config/pins.h"

inline void
uart_setup(void)
{
	usart_set_uart_mode(1);
	usart_set_baudrate(1, 2000000);
	gpio_connect_peripheral(UART_RX);
	gpio_connect_peripheral(UART_TX);
	gpio_dir_out(UART_TX);

	// Use alt 2 pins for uart0
	PERCFG |= PERCFG_U1_ALT2;

	// Enable receive
	// U0CSR |= U0CSR_RE;
	
	// Enable RX interrupt
	// IEN0_URX0IE = 1;

	// Enable TX interrupt
	// IEN2 |= IEN2_UTX0IE;
}
