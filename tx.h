// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "int.h"

// FIXME: Move to common usb interface header
enum ieee802154_status {
	/*
	 * The requested operation was completed successfully.
	 * For a transmission request, this value indicates
	 * a successful transmission.
	 */
	IEEE802154_SUCCESS = 0x0,
	/* The requested operation failed. */
	IEEE802154_MAC_ERROR = 0x1,
	/* The requested operation has been cancelled. */
	IEEE802154_CANCELLED = 0x2,
	/*
	 * Device is ready to poll the coordinator for data in a non beacon
	 * enabled PAN.
	 */
	IEEE802154_READY_FOR_POLL = 0x3,
	/* Wrong frame counter. */
	IEEE802154_COUNTER_ERROR = 0xdb,
	/*
	 * The frame does not conforms to the incoming key usage policy checking
	 * procedure.
	 */
	IEEE802154_IMPROPER_KEY_TYPE = 0xdc,
	/*
	 * The frame does not conforms to the incoming security level usage
	 * policy checking procedure.
	 */
	IEEE802154_IMPROPER_SECURITY_LEVEL = 0xdd,
	/* Secured frame received with an empty Frame Version field. */
	IEEE802154_UNSUPPORTED_LEGACY = 0xde,
	/*
	 * A secured frame is received or must be sent but security is not
	 * enabled in the device. Or, the Auxiliary Security Header has security
	 * level of zero in it.
	 */
	IEEE802154_UNSUPPORTED_SECURITY = 0xdf,
	/* The beacon was lost following a synchronization request. */
	IEEE802154_BEACON_LOST = 0xe0,
	/*
	 * A transmission could not take place due to activity on the
	 * channel, i.e., the CSMA-CA mechanism has failed.
	 */
	IEEE802154_CHANNEL_ACCESS_FAILURE = 0xe1,
	/* The GTS request has been denied by the PAN coordinator. */
	IEEE802154_DENIED = 0xe2,
	/* The attempt to disable the transceiver has failed. */
	IEEE802154_DISABLE_TRX_FAILURE = 0xe3,
	/*
	 * The received frame induces a failed security check according to
	 * the security suite.
	 */
	IEEE802154_FAILED_SECURITY_CHECK = 0xe4,
	/*
	 * The frame resulting from secure processing has a length that is
	 * greater than aMACMaxFrameSize.
	 */
	IEEE802154_FRAME_TOO_LONG = 0xe5,
	/*
	 * The requested GTS transmission failed because the specified GTS
	 * either did not have a transmit GTS direction or was not defined.
	 */
	IEEE802154_INVALID_GTS = 0xe6,
	/*
	 * A request to purge an MSDU from the transaction queue was made using
	 * an MSDU handle that was not found in the transaction table.
	 */
	IEEE802154_INVALID_HANDLE = 0xe7,
	/* A parameter in the primitive is out of the valid range.*/
	IEEE802154_INVALID_PARAMETER = 0xe8,
	/* No acknowledgment was received after aMaxFrameRetries. */
	IEEE802154_NO_ACK = 0xe9,
	/* A scan operation failed to find any network beacons.*/
	IEEE802154_NO_BEACON = 0xea,
	/* No response data were available following a request. */
	IEEE802154_NO_DATA = 0xeb,
	/* The operation failed because a short address was not allocated. */
	IEEE802154_NO_SHORT_ADDRESS = 0xec,
	/*
	 * A receiver enable request was unsuccessful because it could not be
	 * completed within the CAP.
	 */
	IEEE802154_OUT_OF_CAP = 0xed,
	/*
	 * A PAN identifier conflict has been detected and communicated to the
	 * PAN coordinator.
	 */
	IEEE802154_PAN_ID_CONFLICT = 0xee,
	/* A coordinator realignment command has been received. */
	IEEE802154_REALIGNMENT = 0xef,
	/* The transaction has expired and its information discarded. */
	IEEE802154_TRANSACTION_EXPIRED = 0xf0,
	/* There is no capacity to store the transaction. */
	IEEE802154_TRANSACTION_OVERFLOW = 0xf1,
	/*
	 * The transceiver was in the transmitter enabled state when the
	 * receiver was requested to be enabled.
	 */
	IEEE802154_TX_ACTIVE = 0xf2,
	/* The appropriate key is not available in the ACL. */
	IEEE802154_UNAVAILABLE_KEY = 0xf3,
	/*
	 * A SET/GET request was issued with the identifier of a PIB attribute
	 * that is not supported.
	 */
	IEEE802154_UNSUPPORTED_ATTRIBUTE = 0xf4,
	/* Missing source or destination address or address mode. */
	IEEE802154_INVALID_ADDRESS = 0xf5,
	/*
	 * MLME asked to turn the receiver on, but the on time duration is too
	 * big compared to the macBeaconOrder.
	 */
	IEEE802154_ON_TIME_TOO_LONG = 0xf6,
	/*
	 * MLME asaked to turn the receiver on, but the request was delayed for
	 * too long before getting processed.
	 */
	IEEE802154_PAST_TIME = 0xf7,
	/*
	 * The StartTime parameter is nonzero, and the MLME is not currently
	 * tracking the beacon of the coordinator through which it is
	 * associated.
	 */
	IEEE802154_TRACKING_OFF = 0xf8,
	/*
	 * The index inside the hierarchical values in PIBAttribute is out of
	 * range.
	 */
	IEEE802154_INVALID_INDEX = 0xf9,
	/*
	 * The number of PAN descriptors discovered during a scan has been
	 * reached.
	 */
	IEEE802154_LIMIT_REACHED = 0xfa,
	/*
	 * The PIBAttribute parameter specifies an attribute that is a read-only
	 * attribute.
	 */
	IEEE802154_READ_ONLY = 0xfb,
	/*
	 * A request to perform a scan operation failed because the MLME was
	 * in the process of performing a previously initiated scan operation.
	 */
	IEEE802154_SCAN_IN_PROGRESS = 0xfc,
	/* The outgoing superframe overlaps the incoming superframe. */
	IEEE802154_SUPERFRAME_OVERLAP = 0xfd,
	/* Any other error situation. */
	IEEE802154_SYSTEM_ERROR = 0xff,
};

void
tx_usb_intr_handler();

void
tx_radio_intr_handler(u8 flags);

void
usb_status_send(u8 status);

void
tx_setup();

void
tx_set_csma_params(u16 packed_params);

__bit
tx_prepare(u8 msdu_len);

void
tx_csma();

void
tx_now();
