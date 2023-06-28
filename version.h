// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#ifndef GIT_VERSION_MAJOR
#define GIT_VERSION_MAJOR 0
#endif

#ifndef GIT_VERSION_MINOR
#define GIT_VERSION_MINOR 0
#endif

#define BCD_DIGIT(_n) ((_n)%10)
#define BCD_2DIGITS(_n) ( (BCD_DIGIT((_n)/10)<<4) | BCD_DIGIT(_n) )

#define BCD_VERSION (((u16)BCD_2DIGITS(GIT_VERSION_MAJOR) << 8) | BCD_2DIGITS(GIT_VERSION_MINOR))

#ifndef GIT_VERSION_STR
#define GIT_VERSION_STR "<unknown version>"
#endif
