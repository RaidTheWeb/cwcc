

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
	.text
puti:
	push	bp
	mov	bp, sp
	mov	[32:bp:-4], di
	sub	sp, 16
	push	r11
	push	r12
	push	r13
	push	r14
	mov	r11, [32:bp:-4]
	mov	di, r11
	call	printint
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
	push	r11
	push	r12
	push	r13
	push	r14
	mov	r11, 10
	mov	di, r11
	call	printchar
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
L6:
	add	sp, 16
	pop	bp
	ret
	.text
putc:
	push	bp
	mov	bp, sp
	mov	[8:bp:-4], di
	sub	sp, 16
	push	r11
	push	r12
	push	r13
	push	r14
	mov	r11,[8:bp:-4]
	mov	di, r11
	call	printchar
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
	push	r11
	push	r12
	push	r13
	push	r14
	mov	r11, 10
	mov	di, r11
	call	printchar
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
L7:
	add	sp, 16
	pop	bp
	ret
	.text
main:
	push	bp
	mov	bp, sp
	sub	sp, 16
L9:
	push	r11
	push	r12
	push	r13
	push	r14
	call	getchar
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
	mov	[8:bp:-4], r11
	test	r11, r11
	je	L10
	push	r11
	push	r12
	push	r13
	push	r14
	mov	r11,[8:bp:-4]
	mov	di, r11
	call	printchar
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
	jmp	L9
L10:
	mov	r11, 0
	mov	ax, r11
	jmp	L8
L8:
	add	sp, 16
	pop	bp
	ret
