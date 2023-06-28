// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// PORT 0 --------------------------------------------------
#define LED_GREEN_PORT 0
#define LED_GREEN_PIN  0

#define DBG_USB_INTR_PORT 0
#define DBG_USB_INTR_PIN  4

#define DBG_CSPRUNNING_PORT 0
#define DBG_CSPRUNNING_PIN  5

#define DBG_USB_RECV_PORT 0
#define DBG_USB_RECV_PIN  6

#define DBG_USB_SEND_PORT 0
#define DBG_USB_SEND_PIN  7

// PORT 1 --------------------------------------------------
#define USB_DPLUS_PORT 1
#define USB_DPLUS_PIN  0

#define LED_RED_PORT 1
#define LED_RED_PIN  1

// FIXME: output on button input
#define OBS_INTR_PORT 1
#define OBS_INTR_PIN  2
#define BTN0_PORT 1
#define BTN0_PIN  2

// FIXME: output on button input
#define OBS_TX_ACTIVE_PORT 1
#define OBS_TX_ACTIVE_PIN  3
#define BTN1_PORT 1
#define BTN1_PIN  3

#define OBS_DATA_PORT 1
#define OBS_DATA_PIN  4

#define OBS_CLK_PORT 1
#define OBS_CLK_PIN  5

#define UART_TX_PORT 1
#define UART_TX_PIN  6

#define UART_RX_PORT 1
#define UART_RX_PIN  7

// PORT 2 --------------------------------------------------

#define ISP_DATA_PORT 2
#define ISP_DATA_PIN 1

#define ISP_CLK_PORT 2
#define ISP_CLK_PIN 2
