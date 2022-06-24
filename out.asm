

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
setpixel:
	push	bp
	mov	bp, sp
	mov	[32:bp:-4], di
	mov	[32:bp:-8], si
	mov	[32:bp:-12], dx
	sub	sp, 32
	mov	r11, [32:bp:-4]
	mov	r12, 4
	mul	r11, r12
	mov	r12, [32:bp:-8]
	mov	r13, 2560
	mul	r12, r13
	add	r11, r12
	mov	[32:bp:-16], r11
	mov	r11, 768
	mov	r12, [32:bp:-16]
	add	r11, r12
	mov	[32:bp:-20], r11
	mov	r11, [32:bp:-12]
	mov	r12, [32:bp:-20]
	mov	[32:r12], r11
	push	r11
	push	r12
	push	r13
	push	r14
	mov	r11, [32:bp:-20]
	mov	r11, [32:r11]
	mov	di, r11
	call	puti
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
L8:
	add	sp, 32
	pop	bp
	ret
	.text
drawchar:
	push	bp
	mov	bp, sp
	mov	[8:bp:-4], di
	mov	[32:bp:-8], si
	mov	[32:bp:-12], dx
	mov	[32:bp:-16], cx
	mov	[32:bp:-20], r8
	sub	sp, 48
	mov	r11, [32:bp:-8]
	mov	[32:bp:-24], r11
	lea	r11, [32:bitmap_font]
	mov	r12,[8:bp:-4]
	mov	r13, 16
	mul	r12, r13
	add	r11, r12
	mov	r11, [8:r11]
	mov	[8:bp:-28], r11
	lea	r11, [32:bp:-28]
	mov	[32:bp:-32], r11
	mov	r11, 0
	mov	[32:bp:-36], r11
L10:
	mov	r11, [32:bp:-36]
	mov	r12, 16
	cmp	r11, r12
	jge	L11
	mov	r11, 7
	mov	[32:bp:-40], r11
L12:
	mov	r11, [32:bp:-40]
	mov	r12, 0
	cmp	r11, r12
	jl	L13
	mov	r11, [32:bp:-32]
	mov	r12, [32:bp:-36]
	add	r11, r12
	mov	r11, [8:r11]
	mov	r12, [32:bp:-40]
	shr	r11, r12
	mov	[32:bp:-44], r12
	push	r11
	push	r12
	push	r13
	push	r14
	mov	r11, [32:bp:-44]
	mov	r12, 1
	and	r11, r12
	mov	r13, [32:bp:-16]
	mov	r12, r13
	jmp	L15
L14:
	mov	r13, [32:bp:-20]
	mov	r12, r13
L15:
	mov	dx, r12
	mov	r12, [32:bp:-12]
	mov	si, r12
	mov	r12, [32:bp:-8]
	lea	r13, [32:bp:-8]
	add	[32:r13], 1
	mov	di, r12
	call	setpixel
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r12, ax
	mov	r11, [32:bp:-40]
	lea	r12, [32:bp:-40]
	add	[32:r12], -1
	jmp	L12
L13:
	mov	r11, [32:bp:-12]
	lea	r12, [32:bp:-12]
	add	[32:r12], 1
	mov	r11, [32:bp:-24]
	mov	[32:bp:-8], r11
	mov	r11, [32:bp:-36]
	lea	r12, [32:bp:-36]
	add	[32:r12], 1
	jmp	L10
L11:
L9:
	add	sp, 48
	pop	bp
	ret
	.text
main:
	push	bp
	mov	bp, sp
	sub	sp, 0
	push	r11
	push	r12
	push	r13
	push	r14
	mov	r11, 16777215
	mov	dx, r11
	mov	r11, 100
	mov	si, r11
	mov	r11, 200
	mov	di, r11
	call	setpixel
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
	push	r11
	push	r12
	push	r13
	push	r14
	mov	r11, 760
	mov	r11, [32:r11]
	mov	di, r11
	call	puti
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
	mov	r11, 0
	mov	ax, r11
	jmp	L16
L16:
	add	sp, 0
	pop	bp
	ret


.data
bitmap_font:
    #incbin "../audr32/vm/src/bitmap_font.fnt"
.te