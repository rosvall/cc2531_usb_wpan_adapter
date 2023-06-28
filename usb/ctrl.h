// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "../bsp/bits.h"
#include "../int.h"

#define USB_FULLSPEED_MAXPKTSIZE 64

struct usb_setup {
	u8 bmRequestType;
	u8 bRequest;
	union {
		u16 wValue;
		struct {
			u8 idx;
			u8 type;
		} desc;
	};
	u16 wIndex;
	u16 wLength;
};

// usb_request.bmRequestType.recipient
enum usb_rt_recipient {
	USB_RT_DEV  = 0u,
	USB_RT_INTF = 1u,
	USB_RT_EP   = 2u,
};

// usb_request.bmRequestType.type
enum usb_rt_type {
	USB_RT_STD    = (0u << 5),
	USB_RT_CLASS  = (1u << 5),
	USB_RT_VENDOR = (2u << 5),
};

// bmRequestType.direction
enum usb_rt_direction {
	USB_RT_DIR_OUT = (0u << 7),
	USB_RT_DIR_IN  = (1u << 7),
};

#define USB_EP_IS_IN(_ep) (u8)((_ep) & USB_RT_DIR_IN)
#define USB_EP_INDEX(_ep) (u8)((_ep) & 0xf)

#define USB_RT_DIR_MASK BITMASK(1, 7)
#define USB_RT_TYPE_MASK BITMASK(2, 5)
#define USB_RT_RCP_MASK BITMASK(5, 0)

enum usb_rt {
	USB_RT_STD_DEV_IN     = USB_RT_STD    | USB_RT_DEV   | USB_RT_DIR_IN,
	USB_RT_STD_DEV_OUT    = USB_RT_STD    | USB_RT_DEV   | USB_RT_DIR_OUT,
	USB_RT_STD_INTF_IN    = USB_RT_STD    | USB_RT_INTF  | USB_RT_DIR_IN,
	USB_RT_STD_INTF_OUT   = USB_RT_STD    | USB_RT_INTF  | USB_RT_DIR_OUT,
	USB_RT_STD_EP_IN      = USB_RT_STD    | USB_RT_EP    | USB_RT_DIR_IN,
	USB_RT_STD_EP_OUT     = USB_RT_STD    | USB_RT_EP    | USB_RT_DIR_OUT,
	USB_RT_CLASS_INTF_IN  = USB_RT_CLASS  | USB_RT_INTF  | USB_RT_DIR_IN,
	USB_RT_CLASS_INTF_OUT = USB_RT_CLASS  | USB_RT_INTF  | USB_RT_DIR_OUT,
	USB_RT_VENDOR_DEV_IN  = USB_RT_VENDOR | USB_RT_DEV   | USB_RT_DIR_IN,
	USB_RT_VENDOR_DEV_OUT = USB_RT_VENDOR | USB_RT_DEV   | USB_RT_DIR_OUT,
};

enum usb_req_standard {
	USB_REQ_GET_STATUS        =  0u,
	USB_REQ_CLEAR_FEATURE     =  1u,
	USB_REQ_SET_FEATURE       =  3u,
	USB_REQ_SET_ADDRESS       =  5u,
	USB_REQ_GET_DESCRIPTOR    =  6u,
	USB_REQ_SET_DESCRIPTOR    =  7u,
	USB_REQ_GET_CONFIGURATION =  8u,
	USB_REQ_SET_CONFIGURATION =  9u,
	USB_REQ_GET_INTF          = 10u,
	USB_REQ_SET_INTF          = 11u,
};

enum usb_vendor_req {
	USB_REQ_VENDOR_XDATA_READ  =  0u,
	USB_REQ_VENDOR_XDATA_WRITE =  1u,
	USB_REQ_VENDOR_FIFO_READ   =  2u,
	USB_REQ_VENDOR_FIFO_WRITE  =  3u,
	USB_REQ_VENDOR_TX          =  4u,
	USB_REQ_VENDOR_SET_CSMA    =  5u,
};

enum usb_req_dfu {
	USB_REQ_DFU_DETACH        =  0u,
	USB_REQ_DFU_DNLOAD        =  1u,
	USB_REQ_DFU_UPLOAD        =  2u,
	USB_REQ_DFU_GETSTATUS     =  3u,
	USB_REQ_DFU_CLRSTATUS     =  4u,
	USB_REQ_DFU_GETSTATE      =  5u,
	USB_REQ_DFU_ABORT         =  6u,
};

enum usb_feature {
	USB_FEATURE_ENDPOINT_HALT        = 0u,
	USB_FEATURE_DEVICE_REMOTE_WAKEUP = 1u,
	USB_FEATURE_TEST_MODE            = 2u,
};

