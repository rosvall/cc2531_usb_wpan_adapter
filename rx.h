// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "int.h"

void
rx_radio_intr_handler(u8 flags);

void
rx_usb_intr_handler();

void
rx_setup();
