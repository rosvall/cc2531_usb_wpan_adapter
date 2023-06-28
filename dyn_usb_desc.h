// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "int.h"

void
dyn_usb_desc_init();

const void __xdata *
dyn_usb_desc_get(u16 wValue);
