// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "bsp/interrupts.h"

INTERRUPT(usb_intr_handler, INTR_P2INT_USB_I2C);

void
usb_init();
