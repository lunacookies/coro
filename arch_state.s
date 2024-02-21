.global _save_arch_state
.align 2
_save_arch_state:
	str	x1, [x0], #8

	mov	x9, sp
	str	x9, [x0], #8

	stp	x19, x20, [x0], #16
	stp	x21, x22, [x0], #16
	stp	x23, x24, [x0], #16
	stp	x25, x26, [x0], #16
	stp	x27, x28, [x0], #16
	stp	x29, x30, [x0], #16

	stp	d8, d9, [x0], #16
	stp	d10, d11, [x0], #16
	stp	d12, d13, [x0], #16
	stp	d14, d15, [x0], #16

	ret

.global _restore_arch_state
.align 2
_restore_arch_state:
	ldr	x9, [x0], #8

	ldr	x10, [x0], #8
	mov	sp, x10

	ldp	x19, x20, [x0], #16
	ldp	x21, x22, [x0], #16
	ldp	x23, x24, [x0], #16
	ldp	x25, x26, [x0], #16
	ldp	x27, x28, [x0], #16
	ldp	x29, x30, [x0], #16

	ldp	d8, d9, [x0], #16
	ldp	d10, d11, [x0], #16
	ldp	d12, d13, [x0], #16
	ldp	d14, d15, [x0], #16

	br	x9

.global _swap_arch_state
.align 2
_swap_arch_state:
	stp	x29, x30, [sp, #-0x10]!
	mov	x29, sp

	mov	x10, x1
	adr	x1, .L.restore_point
	bl	_save_arch_state

	mov	x0, x10
	bl	_restore_arch_state

.L.restore_point:
	ldp	x29, x30, [sp], #0x10
	ret
