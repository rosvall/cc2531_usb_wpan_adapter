// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// Note: USB VID/PID defined in Makefile

enum {
	USB_STRING_DESC_LANGID = 0,

	USB_STRING_DESC_SERIAL_NUM,
	USB_STRING_DESC_MANUFACTURER,
	USB_STRING_DESC_PRODUCT,
	USB_STRING_DESC_DFU_IF,

	USB_STRING_DESC_FW_VERSION = 0x80,
};

enum {
	USB_INTERFACE_PROTO_RUNTIME = 0x01,
	USB_INTERFACE_PROTO_DFU = 0x02,
};
