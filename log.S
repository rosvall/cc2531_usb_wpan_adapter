; SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
;
; SPDX-License-Identifier: GPL-3.0-or-later

; Really simple and fast logging over uart

; The main point is to be fast and reentrant, to facilitate logging
; even from nested interrupts without causing stack overflows 
; or any other annoying problems.

.module log
.optsdcc -mmcs51 --model-small

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; USART registers and register bits
U0CSR		= 0x86
U1CSR		= 0xf8
U0DBUF		= 0xc1
U1DBUF		= 0xf9
U0UCR		= 0xc4
U1UCR		= 0xfb
U0GCR		= 0xc5
U1GCR		= 0xfc

U1CSR_ACTIVE	= 0xf8
U1CSR_TX_BYTE	= 0xf9
U1CSR_RX_BYTE	= 0xfa
U1CSR_ERR	= 0xfb
U1CSR_FE	= 0xfc
U1CSR_SLAVE	= 0xfd
U1CSR_RE	= 0xfe
U1CSR_MODE	= 0xff

; We're using USART1, because it has bit-adressable status bits
uart_busy	= U1CSR_ACTIVE
uart_fifo	= U1DBUF

; Handy character names for prefixes/suffixes
_sp	= '  
_cl	= ':
_lf	= '\n
_eof	= '\0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.area CSEG    (CODE)

.macro UART_TX EXPR
	; Transmit byte
	mov	uart_fifo, EXPR
	; Wait until sent
	jb	uart_busy, .
.endm

_putchar::
	UART_TX	dpl
	ret

_puts::
	mov	b, #_lf
	sjmp	puts

_puts_sp_cl::
	mov	b, #_cl
	sjmp	puts

_puts_nolf::
	mov	b, #_eof
	sjmp	puts


.macro define_log_str PREFIX, SUFFIX
_log_'PREFIX'_s'SUFFIX::
	mov	a, #''PREFIX
	mov	b, #SUFFIX
	sjmp	log_pref_s
.endm

define_log_str	D, _lf
define_log_str	I, _lf
define_log_str	W, _lf
define_log_str	E, _lf
define_log_str	D, _cl
define_log_str	I, _cl
define_log_str	W, _cl
define_log_str	E, _cl

log_pref_s:
	UART_TX	a
	UART_TX	#_cl
	UART_TX	#_sp
	; fallthrough to puts
puts:
	puts_loop:
		clr	a
		movc	a, @a+dptr
		jz	put_char_b
		inc	dptr
		UART_TX	a
	sjmp	puts_loop
put_char_b:
	mov	a, b
	jz	put_char_b_skip
	UART_TX	a
put_char_b_skip:
	ret

.macro 	HEXNYB R
	; Convert low nybble to ascii hex character (0-9,A-F)
	anl	R, #0x0f
	add	R, #0x90
	da	R
	addc    R, #0x40
	da	R
.endm

puthex16:
	; MSD
	mov	a, dph
	swap	a
	HEXNYB	a
	UART_TX	a
	; LSD
	mov	a, dph
	HEXNYB	a
	UART_TX	a
	; fallthrough to puthex8
puthex8:
	; MSD
	mov	a, dpl
	swap	a
	HEXNYB	a
	UART_TX	a
	; LSD
	mov	a, dpl
	HEXNYB	a
	UART_TX	a
	sjmp	put_char_b

_puthex8::
	mov	b, #'\n
	sjmp	puthex8

_puthex8_nolf::
	mov	b, #0
	sjmp	puthex8

_puthex16::
	mov	b, #'\n
	sjmp	puthex16

_puthex16_nolf::
	mov	b, #0
	sjmp	puthex16
