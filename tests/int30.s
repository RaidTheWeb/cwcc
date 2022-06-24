	.text
	call main
L0:
	jmp L0


printint:
	push	bp
	mov	bp, sp
	sub	sp, 8
	mov	r10, 0x07
	mov	dx, di
	mov	bx, 0x0F00
	int	0x10
	add	sp, 8
	pop	bp
	ret


printchar:
	push	bp
	mov	bp, sp
	sub	sp, 8
	mov	r10, 0x01
	mov	dx, di
	mov	bx, 0x0F00
	int	0x10
	add	sp, 8
	pop	bp
	ret


puts:
	push	bp
	mov	bp, sp
	sub	sp, 8
	puts_print_loop:
	mov	r10, 0x01
	cmp	[8:di], 0
	je	puts_print_loop_end
	mov	dx, [8:di]
	push	di
	mov	bx, 0x0F00
	int	0x10
	pop	di
	add	di, 1
	jmp	puts_print_loop
	puts_print_loop_end:
	add	sp, 8
	pop	bp
	ret


getchar:
	push	bp
	mov	bp, sp
	sub	sp, 8
	getc_loop:
	mov	r10, 0x01
	int 0x16
	jnz	getc_loop_end
	jmp	getc_loop
	getc_loop_end:
	mov r10, 0x02
	int 0x16
	mov	ax, r9
	add	sp, 8
	pop	bp
	ret
