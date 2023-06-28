; SPDX-FileCopyrightText: 2023 Andreas Sig Rosvall
;
; SPDX-License-Identifier: GPL-3.0-or-later

.module dyn_usb_desc_serial

MPAGE     = 0x93

INFOPAGE  = 0x7800
INFOPAGE_PERM_ADDR = INFOPAGE + 0xC

.area CSEG (CODE)

_render_serial_desc::

	push MPAGE

	mov a, #(2 + 32)
	movx @dptr, a
	inc dptr
	mov a, #0x03
	movx @dptr, a
	inc dptr

	mov MPAGE, #(INFOPAGE_PERM_ADDR >> 8)
	mov r0, #(INFOPAGE_PERM_ADDR + 7)
	mov b, #16

	loop:
		movx a, @r0
		jnb b.0, msb
			swap a
			dec r0
		msb:
		swap a

		anl a, #0x0f
		add a, #0x90
		da a
		addc a, #0x40
		da a

		movx @dptr, a
		inc dptr
		clr a
		movx @dptr, a
		inc dptr

	djnz b, loop

	pop MPAGE

	ret
