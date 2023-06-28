// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "render_serial_desc.h"
#include "usb/descriptor.h"
#include "config/usb.h"

#include "dyn_usb_desc.h"

static __xdata struct {
	struct descriptor_hdr hdr;
	u16 wstring[16];
} serial_desc;

void
dyn_usb_desc_init()
{
	render_serial_desc(&serial_desc);
}

const void __xdata *
dyn_usb_desc_get(u16 wValue)
{
	u8 type = wValue>>8;
	u8 index = wValue;

	if (type == USB_DT_STRING) {
		if (index == USB_STRING_DESC_SERIAL_NUM)
			return &serial_desc;
	}

	return NULL;
}
