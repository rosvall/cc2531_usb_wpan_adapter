// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "bsp/usb.h"
#include "usb/ctrl.h"

#define CTRL_EP  0
#define INT_EP   1
#define RXPKT_EP 5

#define CTRL_EP_MAXPKTSIZE  USB_EP0_FIFO_SIZE
#define RXPKT_EP_MAXPKTSIZE USB_FULLSPEED_MAXPKTSIZE
#define INT_EP_MAXPKTSIZE   8

enum {
	USB_INTERFACE_NUM_WPAN = 0,
	USB_INTERFACE_NUM_DFU  = 1,

	USB_INTERFACE_COUNT,
};

#define MANUFACTURER "Andreas Rosvall"
#define PRODUCT      "CC2531 USB WPAN Adapter"
#define FW_VERSION   GIT_VERSION_STR
