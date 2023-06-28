// SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include "int.h"
#include <compiler.h>

#define LOG_ERROR 1
#define LOG_WARN  2
#define LOG_INFO  3
#define LOG_DEBUG 4

#ifndef LOG_LEVEL
	#define LOG_LEVEL LOG_INFO
#endif

#if LOG_LEVEL >= LOG_DEBUG
	#define LOGD(_str) __LOG_STR(D, _str, lf)
	#define LOGDX8(_str, _n)  __LOG_STR_HEX8(D, _str, _n)
	#define LOGDX16(_str, _n) __LOG_STR_HEX16(D, _str, _n)
#else
	#define LOGD(...)
	#define LOGDX8(_str, _n)
	#define LOGDX16(_str, _n)
#endif

#if LOG_LEVEL >= LOG_INFO
	#define LOGI(_str) __LOG_STR(I, _str, lf)
	#define LOGIX8(_str, _n)  __LOG_STR_HEX8(I, _str, _n)
	#define LOGIX16(_str, _n) __LOG_STR_HEX16(I, _str, _n)
#else
	#define LOGI(...)
	#define LOGIX8(_str, _n)
	#define LOGIX16(_str, _n)
#endif

#if LOG_LEVEL >= LOG_WARN
	#define LOGW(_str) __LOG_STR(W, _str, lf)
	#define LOGWX8(_str, _n)  __LOG_STR_HEX8(W, _str, _n)
	#define LOGWX16(_str, _n) __LOG_STR_HEX16(W, _str, _n)
#else
	#define LOGW(...)
	#define LOGWX8(_str, _n)
	#define LOGWX16(_str, _n)
#endif

#if LOG_LEVEL >= LOG_ERROR
	#define LOGE(_str) __LOG_STR(E, _str, lf)
	#define LOGEX8(_str, _n)  __LOG_STR_HEX8(E, _str, _n)
	#define LOGEX16(_str, _n) __LOG_STR_HEX16(E, _str, _n)
#else
	#define LOGE(...)
	#define LOGEX8(_str, _n)
	#define LOGEX16(_str, _n)
#endif

#define __LOG_STR(_lvl_char, _str, _endchar) \
	log_ ## _lvl_char ## _s_ ## _endchar (_str)

#define __LOG_STR_HEX8(_lvl_char, _str, _n)                                \
	{                                                                          \
		__LOG_STR(_lvl_char, _str, cl);                                       \
		puthex8(_n);                                                           \
	}

#define __LOG_STR_HEX16(_lvl_char, _str, _n)                               \
	{                                                                          \
		__LOG_STR(_lvl_char, _str, cl);                                       \
		puthex16(_n);                                                          \
	}

#pragma callee_saves putchar
void
putchar(char c);

#pragma callee_saves puts
void
puts(const __code char * str);

#pragma callee_saves puts
void
puts(const __code char * str);

#pragma callee_saves puts_nolf
void
puts_nolf(const __code char * str);

#pragma callee_saves puts_sp_cl
void
puts_sp_cl(const __code char * str);

#pragma callee_saves puthex8
void
puthex8(u8 n);

#pragma callee_saves puthex16
void
puthex16(u16 n);

#pragma callee_saves log_D_s_lf
void
log_D_s_lf(const __code char * str);

#pragma callee_saves log_I_s_lf
void
log_I_s_lf(const __code char * str);

#pragma callee_saves log_W_s_lf
void
log_W_s_lf(const __code char * str);

#pragma callee_saves log_E_s_lf
void
log_E_s_lf(const __code char * str);

#pragma callee_saves log_D_s_cl
void
log_D_s_cl(const __code char * str);

#pragma callee_saves log_I_s_cl
void
log_I_s_cl(const __code char * str);

#pragma callee_saves log_W_s_cl
void
log_W_s_cl(const __code char * str);

#pragma callee_saves log_E_s_cl
void
log_E_s_cl(const __code char * str);
