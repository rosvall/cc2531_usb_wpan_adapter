// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "bsp/interrupts.h"

INTERRUPT(rferr_isr, INTR_RFERR);
INTERRUPT(rf_isr, INTR_RF);

void
radio_setup(void);

void
radio_stop(void);
