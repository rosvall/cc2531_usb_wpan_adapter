// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "usb/descriptor.h"
#include "config/dfu.h"
#include "config/usb.h"
#include "config/misc.h"
#include "usb_config.h"
#include "version.h"

#define ARRAY_SIZE(_arr) (sizeof(_arr)/sizeof(*(_arr)))

static const struct device_descriptor device_desc = {
	.hdr                = DESC_HDR(device),
	.bcdUSB             = BCD_USB_2_0,
	.bMaxPacketSize0    = CTRL_EP_MAXPKTSIZE,
	.idVendor           = USB_VID,
	.idProduct          = USB_PID,
	.bcdDevice          = BCD_VERSION,
	.iManufacturer      = USB_STRING_DESC_MANUFACTURER,
	.iProduct           = USB_STRING_DESC_PRODUCT,
	.iSerialNumber      = USB_STRING_DESC_SERIAL_NUM,
	.bNumConfigurations = 1,
};

static const struct {
	struct configuration_descriptor configuration;
	struct {
		struct interface_descriptor interface;
		struct endpoint_descriptor endpoints[2];
	} wpan;
	struct {
		struct interface_descriptor interface;
		struct dfu_functional_descriptor functional;
	} dfu;
} configuration_desc = {
	.configuration = {
		.hdr                  = DESC_HDR(configuration),
	    .wTotalLength         = sizeof(configuration_desc),
	    .bNumInterfaces       = 2,
	    .bConfigurationValue  = 1,
	    .bmAttributes         = { .usb1_bus_powered = 1 },
	    .MaxPower             = USB_DESC_MILLIAMPS(CONFIG_VBUS_MAX_CURRENT_MA),
	 },
	.wpan = {
		.interface = {
			.hdr                  = DESC_HDR(interface),
		    .bInterfaceNumber     = USB_INTERFACE_NUM_WPAN,
		    .bAlternateSetting    = 0,
		    .bNumEndpoints        = ARRAY_SIZE(configuration_desc.wpan.endpoints),
		    .bInterfaceClass      = USB_CLASS_VENDOR_SPEC,
		    .bInterfaceSubClass   = USB_SUBCLASS_VENDOR_SPEC,
		    .bInterfaceProtocol   = USB_PROTOCOL_VENDOR_SPEC,
		 },
		.endpoints = {
			{
				.hdr              = DESC_HDR(endpoint),
				.bEndpointAddress = { .number=INT_EP, .in=1 },
				.bmAttributes     = { .transfer_type=TRANSFER_TYPE_INTERRUPT },
				.wMaxPacketSize   = INT_EP_MAXPKTSIZE,
				.bInterval        = 1,
			},
			{
				.hdr              = DESC_HDR(endpoint),
				.bEndpointAddress = { .number=RXPKT_EP, .in=1 },
				.bmAttributes     = { .transfer_type=TRANSFER_TYPE_BULK },
				.wMaxPacketSize   = RXPKT_EP_MAXPKTSIZE,
				.bInterval        = 255,
			},
		},
	},
	.dfu = {
		.interface = {
			.hdr                  = DESC_HDR(interface),
		    .bInterfaceNumber     = USB_INTERFACE_NUM_DFU,
		    .bInterfaceClass      = USB_CLASS_APP_SPEC,
		    .bInterfaceSubClass   = USB_SUBCLASS_DFU,
		    .bInterfaceProtocol   = USB_INTERFACE_PROTO_RUNTIME,
		 },
		.functional = {
			.hdr                      = DESC_HDR(dfu_functional),
			.bitCanDnload             = DFU_CAN_DNLOAD,
			.bitCanUpload             = DFU_CAN_UPLOAD,
			.bitManifestationTolerant = DFU_MANIFESTATION_TOLERANT,
			.bitWillDetach            = DFU_WILL_DETACH,
			.wTransferSize            = DFU_BLOCK_SIZE,
			.wDetachTimeout           = DFU_DETACH_TIMEOUT,
			.bcdDFUVersion            = DFU_BCDDFU,
		},
	},
};

static const struct string_descriptor
	langid_desc       = LANGID_DESC(LANGID_ENGLISH_US),
	manufacturer_desc = STRING_DESC(MANUFACTURER),
	product_desc      = STRING_DESC(PRODUCT),
	fw_version_desc   = STRING_DESC(FW_VERSION);

const void __code *
const_usb_desc_get(u16 wValue)
{
	u8 type = wValue >> 8;
	u8 index = wValue;

	if (type == DESC_TYPE_device)
		return &device_desc;

	if (type == DESC_TYPE_configuration)
		return &configuration_desc;

	if (type == DESC_TYPE_string) {
		if (index == USB_STRING_DESC_LANGID)
			return &langid_desc;

		if (index == USB_STRING_DESC_MANUFACTURER)
			return &manufacturer_desc;

		if (index == USB_STRING_DESC_PRODUCT)
			return &product_desc;

		if (index == USB_STRING_DESC_FW_VERSION)
			return &fw_version_desc;
	}

	return NULL;
}
