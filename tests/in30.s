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
	jmp	L8
	.data
x:	.dword	0
	.text
L8:
	jmp	L9
	.data
y:	.dword	0
	.text
L9:
	jmp	L10
	.data
z:	.dword	0
	.text
L10:
	jmp	L11
	.data
a:	.dword	1
	.text
L11:
	jmp	L12
	.data
aptr:	.dword	0
	.text
L12:
	jmp L14
	.data
L15:
	.byte	0x20
	.byte	0x7c
	.byte	0x20
	.byte	0x00
	.text
L14:
	jmp L16
	.data
L17:
	.byte	0x20
	.byte	0x7c
	.byte	0x20
	.byte	0x00
	.text
L16:
	jmp L18
	.data
L19:
	.byte	0x61
	.byte	0x70
	.byte	0x74
	.byte	0x72
	.byte	0x20
	.byte	0x70
	.byte	0x6f
	.byte	0x69
	.byte	0x6e
	.byte	0x74
	.byte	0x73
	.byte	0x20
	.byte	0x61
	.byte	0x74
	.byte	0x20
	.byte	0x31
	.byte	0x0a
	.byte	0x00
	.text
L18:
	jmp L20
	.data
L21:
	.byte	0x61
	.byte	0x70
	.byte	0x74
	.byte	0x72
	.byte	0x20
	.byte	0x69
	.byte	0x73
	.byte	0x20
	.byte	0x4e
	.byte	0x55
	.byte	0x4c
	.byte	0x4c
	.byte	0x20
	.byte	0x6f
	.byte	0x72
	.byte	0x20
	.byte	0x64
	.byte	0x6f
	.byte	0x65
	.byte	0x73
	.byte	0x6e
	.byte	0x27
	.byte	0x74
	.byte	0x20
	.byte	0x70
	.byte	0x6f
	.byte	0x69
	.byte	0x6e
	.byte	0x74
	.byte	0x20
	.byte	0x61
	.byte	0x74
	.byte	0x20
	.byte	0x31
	.byte	0x0a
	.byte	0x00
	.text
L20:
	jmp L22
	.data
L23:
	.byte	0x61
	.byte	0x70
	.byte	0x74
	.byte	0x72
	.byte	0x20
	.byte	0x70
	.byte	0x6f
	.byte	0x69
	.byte	0x6e
	.byte	0x74
	.byte	0x73
	.byte	0x20
	.byte	0x61
	.byte	0x74
	.byte	0x20
	.byte	0x31
	.byte	0x0a
	.byte	0x00
	.text
L22:
	jmp L24
	.data
L25:
	.byte	0x61
	.byte	0x70
	.byte	0x74
	.byte	0x72
	.byte	0x20
	.byte	0x69
	.byte	0x73
	.byte	0x20
	.byte	0x4e
	.byte	0x55
	.byte	0x4c
	.byte	0x4c
	.byte	0x20
	.byte	0x6f
	.byte	0x72
	.byte	0x20
	.byte	0x64
	.byte	0x6f
	.byte	0x65
	.byte	0x73
	.byte	0x6e
	.byte	0x27
	.byte	0x74
	.byte	0x20
	.byte	0x70
	.byte	0x6f
	.byte	0x69
	.byte	0x6e
	.byte	0x74
	.byte	0x20
	.byte	0x61
	.byte	0x74
	.byte	0x20
	.byte	0x31
	.byte	0x0a
	.byte	0x00
	.text
L24:
	.text
main:
	push	bp
	mov	bp, sp
	sub	sp, 0
	mov	r11, 0
	mov	[32:x], r11
L26:
	mov	r11,[32:x]
	mov	r12, 1
	cmp	r11, r12
	jg	L27
	mov	r11, 0
	mov	[32:y], r11
L28:
	mov	r11,[32:y]
	mov	r12, 1
	cmp	r11, r12
	jg	L29
	mov	r11,[32:x]
	test	r11, r11
	je	L30
	mov	r11,[32:y]
	test	r11, r11
	je	L30
	mov	r11, 1
	jmp	L31
L30:
	mov	r11, 0
L31:
	mov	[32:z], r11
	push	r11
	push	r12
	push	r13
	push	r14
	mov	r11,[32:x]
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
	mov	r11, 32
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
	mov	r11,[32:y]
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
	lea	r11, [32:L15]
	mov	di, r11
	call	puts
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
	push	r11
	push	r12
	push	r13
	push	r14
	mov	r11,[32:z]
	mov	di, r11
	call	puti
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
	mov	r11,[32:y]
	lea	r12, [32:y]
	add	[32:r12], 1
	jmp	L28
L29:
	mov	r11,[32:x]
	lea	r12, [32:x]
	add	[32:r12], 1
	jmp	L26
L27:
	mov	r11, 0
	mov	[32:x], r11
L32:
	mov	r11,[32:x]
	mov	r12, 1
	cmp	r11, r12
	jg	L33
	mov	r11, 0
	mov	[32:y], r11
L34:
	mov	r11,[32:y]
	mov	r12, 1
	cmp	r11, r12
	jg	L35
	mov	r11,[32:x]
	test	r11, r11
	jne	L36
	mov	r11,[32:y]
	test	r11, r11
	jne	L36
	mov	r11, 0
	jmp	L37
L36:
	mov	r11, 1
L37:
	mov	[32:z], r11
	push	r11
	push	r12
	push	r13
	push	r14
	mov	r11,[32:x]
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
	mov	r11, 32
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
	mov	r11,[32:y]
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
	lea	r11, [32:L17]
	mov	di, r11
	call	puts
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
	push	r11
	push	r12
	push	r13
	push	r14
	mov	r11,[32:z]
	mov	di, r11
	call	puti
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
	mov	r11,[32:y]
	lea	r12, [32:y]
	add	[32:r12], 1
	jmp	L34
L35:
	mov	r11,[32:x]
	lea	r12, [32:x]
	add	[32:r12], 1
	jmp	L32
L33:
	mov	r11, 0
	mov	[32:aptr], r11
	mov	r11,[32:aptr]
	test	r11, r11
	je	L40
	mov	r11,[32:aptr]
	mov	r11, [32:r11]
	mov	r12, 1
	cmp	r11, r12
	seteq	r12
	test	r12, r12
	je	L40
	mov	r12, 1
	jmp	L41
L40:
	mov	r12, 0
L41:
	test	r12, r12
	je	L38
	push	r11
	push	r12
	push	r13
	push	r14
	lea	r11, [32:L19]
	mov	di, r11
	call	puts
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
	jmp	L39
L38:
	push	r11
	push	r12
	push	r13
	push	r14
	lea	r11, [32:L21]
	mov	di, r11
	call	puts
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
L39:
	lea	r11, [32:a]
	mov	[32:aptr], r11
	mov	r11,[32:aptr]
	test	r11, r11
	je	L44
	mov	r11,[32:aptr]
	mov	r11, [32:r11]
	mov	r12, 1
	cmp	r11, r12
	seteq	r12
	test	r12, r12
	je	L44
	mov	r12, 1
	jmp	L45
L44:
	mov	r12, 0
L45:
	test	r12, r12
	je	L42
	push	r11
	push	r12
	push	r13
	push	r14
	lea	r11, [32:L23]
	mov	di, r11
	call	puts
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
	jmp	L43
L42:
	push	r11
	push	r12
	push	r13
	push	r14
	lea	r11, [32:L25]
	mov	di, r11
	call	puts
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
L43:
	mov	r11, 0
	mov	ax, r11
	jmp	L13
L13:
	add	sp, 0
	pop	bp
	ret
