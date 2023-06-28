// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "../int.h"

enum usb_desc_type {
	USB_DT_DEVICE = 0x01,
	USB_DT_CONFIGURATION = 0x02,
	USB_DT_STRING = 0x03,
	USB_DT_INTERFACE = 0x04,
	USB_DT_ENDPOINT = 0x05,
	USB_DT_DEVICE_QUALIFIER = 0x06,
	USB_DT_OTHER_SPEED_CONFIG = 0x07,
	USB_DT_INTERFACE_POWER = 0x08,
	USB_DT_DFU_FUNCTIONAL = 0x21,
};

enum usb_class {
	USB_CLASS_PER_INTERFACE = 0x00,
	USB_CLASS_COMM = 0x02,
	USB_CLASS_HID = 0x03,
	USB_CLASS_MASS_STORAGE = 0x08,
	USB_CLASS_HUB = 0x09,
	USB_CLASS_DATA = 0x0A,
	USB_CLASS_APP_SPEC = 0xFE,
	USB_CLASS_VENDOR_SPEC = 0xFF,
};

enum usb_bcd_version {
	BCD_USB_1_0 = 0x0100,
	BCD_USB_1_1 = 0x0110,
	BCD_USB_2_0 = 0x0200,
};

enum transfer_type {
	TRANSFER_TYPE_CONTROL = 0,
	TRANSFER_TYPE_ISOCHRONOUS = 1,
	TRANSFER_TYPE_BULK = 2,
	TRANSFER_TYPE_INTERRUPT = 3,
};

enum usb_langid {
	LANGID_ENGLISH_US = 0x0409,
};

enum usb_subclass {
	USB_SUBCLASS_DFU = 0x01,
	USB_SUBCLASS_VENDOR_SPEC = 0xFF,
};

enum usb_protocol {
	USB_PROTOCOL_VENDOR_SPEC = 0xFF,
};

#define DFU_BCDDFU 0x0100

#define USB_DESC_MILLIAMPS(I) (I / 2)

enum desc_type {
	DESC_TYPE_device = 0x01,
	DESC_TYPE_configuration = 0x02,
	DESC_TYPE_string = 0x03,
	DESC_TYPE_langid = DESC_TYPE_string,
	DESC_TYPE_interface = 0x04,
	DESC_TYPE_endpoint = 0x05,
	DESC_TYPE_device_qualifier = 0x06,
	DESC_TYPE_other_speed_config = 0x07,
	DESC_TYPE_interface_power = 0x08,
	DESC_TYPE_dfu_functional = 0x21,
};

#define DESC_HDR(_type)                                                        \
	{                                                                          \
		.bLength = sizeof(struct _type##_descriptor),                          \
		.bDescriptorType = DESC_TYPE_##_type,                                  \
	}

struct descriptor_hdr {
	u8 bLength;  // Size of this descriptor in bytes
	u8 bDescriptorType;  // enum desc_type
};

struct device_descriptor {
	struct descriptor_hdr hdr;
	u16 bcdUSB;  // Binay Coded Decimal Spec. release
	u8 bDeviceClass;  // Class code assigned by the USB
	u8 bDeviceSubClass;  // Sub-class code assigned by the USB
	u8 bDeviceProtocol;  // Protocol code assigned by the USB
	u8 bMaxPacketSize0;  // Max packet size for EP0
	u16 idVendor;  // Vendor ID
	u16 idProduct;  // Product ID
	u16 bcdDevice;  // Device release number
	u8 iManufacturer;  // Index of manu. string descriptor
	u8 iProduct;  // Index of prod. string descriptor
	u8 iSerialNumber;  // Index of serial number string descriptor
	u8 bNumConfigurations;  // Number of possible configurations
};

struct configuration_descriptor {
	struct descriptor_hdr hdr;
	u16 wTotalLength;  // total length of data returned
	u8 bNumInterfaces;  // number of interfaces for this conf.
	u8 bConfigurationValue;  // value for SetConfiguration resquest
	u8 iConfiguration;  // index of string descriptor
	struct {
		u8:5, remote_wakeup:1, self_powered:1, usb1_bus_powered:1;
	} bmAttributes;
	u8 MaxPower;  // maximum power consumption
};

struct interface_descriptor {
	struct descriptor_hdr hdr;
	u8 bInterfaceNumber;  // Number of interface
	u8 bAlternateSetting;  // value to select alternate setting
	u8 bNumEndpoints;  // Number of EP except EP 0
	u8 bInterfaceClass;  // Class code assigned by the USB
	u8 bInterfaceSubClass;  // Sub-class code assigned by the USB
	u8 bInterfaceProtocol;  // Protocol code assigned by the USB
	u8 iInterface;  // Index of string descriptor
};

struct endpoint_descriptor {
	struct descriptor_hdr hdr;
	struct {
		u8 number:4, :3, in:1;
	} bEndpointAddress;  // Address of the endpoint
	struct {
		u8 transfer_type:2, sync_type:2, usage_type:2, :2;
	} bmAttributes;
	u16 wMaxPacketSize;  // Maximum packet size for this EP
	u8 bInterval;  // Interval for polling EP in ms
	// u8  bRefresh;
	// u8  bSynchAddress;
};

struct string_descriptor {
	struct descriptor_hdr hdr;
	unsigned wstring[];  // UTF-16 string
};

#define LANGID_DESC(_langid)                                                   \
	{                                                                          \
		.hdr = {                                                               \
			.bLength = 4,                                                      \
			.bDescriptorType = DESC_TYPE_string,                               \
		},                                                                     \
		.wstring = { _langid, },                                               \
	}

#define TO_UTF16(_str) u"" _str

#define STRING_DESC(_str)                                                      \
	{                                                                          \
		.hdr = {                                                               \
			.bLength = sizeof(TO_UTF16(_str)),                                 \
			.bDescriptorType = DESC_TYPE_string,                               \
		},                                                                     \
		.wstring = TO_UTF16(_str),                                             \
	}

struct dfu_functional_descriptor {
	struct descriptor_hdr hdr;
	u8 bitCanDnload             :1,
	   bitCanUpload             :1,
	   bitManifestationTolerant :1,
	   bitWillDetach            :1,
	                            :4;
	u16 wDetachTimeout;
	u16 wTransferSize;
	u16 bcdDFUVersion;
};

inline u8
usb_desc_total_len(const void __xdata * desc)
{
	// Annoying special case for configuration descriptor

	__xdata struct configuration_descriptor * cd
	    = (__xdata struct configuration_descriptor *)desc;

	u8 len = cd->hdr.bLength;

	if (cd->hdr.bDescriptorType == USB_DT_CONFIGURATION)
		len =  cd->wTotalLength;

	return len;
}
