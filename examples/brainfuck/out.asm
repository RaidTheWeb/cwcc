

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
	jmp L9
	.data
L10:
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x2c
	.byte	0x2e
	.byte	0x00
	.text
L9:
	.text
main:
	push	bp
	mov	bp, sp
	sub	sp, 416
	lea	r11, [32:L10]
	mov	[32:bp:-404], r11
	mov	r11, 0
	mov	[32:bp:-408], r11
	mov	r11, 0
	mov	[32:bp:-412], r11
L11:
	mov	r11, [32:bp:-404]
	mov	r11, [8:r11]
	test	r11, r11
	je	L12
	mov	r11, [32:bp:-404]
	mov	r11, [8:r11]
	jmp	L13
L15:
	lea	r11, [32:bp:-400]
	mov	r12, [32:bp:-412]
	shl	r12, 2
	add	r11, r12
	mov	r11, [32:r11]
	mov	r12, 1
	add	r11, r12
	lea	r12, [32:bp:-400]
	mov	r13, [32:bp:-412]
	shl	r13, 2
	add	r12, r13
	mov	[32:r12], r11
	jmp	L14
L16:
	lea	r11, [32:bp:-400]
	mov	r12, [32:bp:-412]
	shl	r12, 2
	add	r11, r12
	mov	r11, [32:r11]
	mov	r12, 1
	sub	r11, r12
	lea	r12, [32:bp:-400]
	mov	r13, [32:bp:-412]
	shl	r13, 2
	add	r12, r13
	mov	[32:r12], r11
	jmp	L14
L17:
	mov	r11, [32:bp:-412]
	lea	r12, [32:bp:-412]
	add	[32:r12], 1
	jmp	L14
L18:
	mov	r11, [32:bp:-412]
	lea	r12, [32:bp:-412]
	add	[32:r12], -1
	jmp	L14
L19:
	push	r11
	push	r12
	push	r13
	push	r14
	lea	r11, [32:bp:-400]
	mov	r12, [32:bp:-412]
	shl	r12, 2
	add	r11, r12
	mov	r11, [32:r11]
	mov	di, r11
	call	printchar
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
	jmp	L14
L20:
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
	lea	r12, [32:bp:-400]
	mov	r13, [32:bp:-412]
	shl	r13, 2
	add	r12, r13
	mov	[32:r12], r11
	jmp	L14
L21:
	jmp	L14
	jmp	L14
L22:
	cmp	r11, 43
	je	L15
	cmp	r11, 45
	je	L16
	cmp	r11, 62
	je	L17
	cmp	r11, 60
	je	L18
	cmp	r11, 46
	je	L19
	cmp	r11, 44
	je	L20
	jmp	L21
L13:
	jmp	L22
L14:
	mov	r11, [32:bp:-404]
	lea	r12, [32:bp:-404]
	add	[32:r12], 1
	jmp	L11
L12:
	mov	r11, 0
	mov	ax, r11
	jmp	L8
L8:
	add	sp, 416
	pop	bp
	ret
