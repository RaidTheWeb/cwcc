	.text
	call main
L0:
	jmp L0


printint:
	push	bp
	mov	bp, sp
	sub	sp, 8
	mov	r10, 0x07
	mov	dx, r0
	mov	cx, 1
	mov	bx, 0x0F00
	int	0x10
	add	sp, 8
	pop	bp
	ret
printuint:
	push	bp
	mov	bp, sp
	sub	sp, 8
	mov	r10, 0x07
	mov	dx, r0
	mov	cx, 0
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
	mov	dx, r0
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
	cmp	[8:r0], 0
	je	puts_print_loop_end
	mov	dx, [8:r0]
	push	r0
	mov	bx, 0x0F00
	int	0x10
	pop	r0
	add	r0, 1
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
	int	0x16
	jnz	getc_loop_end
	jmp	getc_loop
	getc_loop_end:
	mov	r10, 0x02
	int	0x16
	cmp	r9, 0x00
	je	getc_loop
	mov	ax, r9
	add	sp, 8
	pop	bp
	ret
	.text
puti:
	push	bp
	mov	bp, sp
	mov	[32:bp:-4], r0
	sub	sp, 16
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15
	push	r8
	push	r0
	push	r1
	push	r2
	push	r3
	push	r4
	push	r5
	push	r6
	push	r7
	mov	r11, [32:bp:-4]
	mov	r0, r11
	call	printint
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0
	pop	r8
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15
	push	r8
	push	r0
	push	r1
	push	r2
	push	r3
	push	r4
	push	r5
	push	r6
	push	r7
	mov	r11, 10
	mov	r0, r11
	call	printchar
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0
	pop	r8
	pop	r15
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
putui:
	push	bp
	mov	bp, sp
	mov	[32:bp:-4], r0
	sub	sp, 16
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15
	push	r8
	push	r0
	push	r1
	push	r2
	push	r3
	push	r4
	push	r5
	push	r6
	push	r7
	mov	r11, [32:bp:-4]
	mov	r0, r11
	call	printuint
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0
	pop	r8
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15
	push	r8
	push	r0
	push	r1
	push	r2
	push	r3
	push	r4
	push	r5
	push	r6
	push	r7
	mov	r11, 10
	mov	r0, r11
	call	printchar
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0
	pop	r8
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
L8:
	add	sp, 16
	pop	bp
	ret
	.text
putc:
	push	bp
	mov	bp, sp
	mov	[8:bp:-4], r0
	sub	sp, 16
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15
	push	r8
	push	r0
	push	r1
	push	r2
	push	r3
	push	r4
	push	r5
	push	r6
	push	r7
	mov	r11,[8:bp:-4]
	mov	r0, r11
	call	printchar
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0
	pop	r8
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15
	push	r8
	push	r0
	push	r1
	push	r2
	push	r3
	push	r4
	push	r5
	push	r6
	push	r7
	mov	r11, 10
	mov	r0, r11
	call	printchar
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0
	pop	r8
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
L9:
	add	sp, 16
	pop	bp
	ret
	.text
setpixel:
	push	bp
	mov	bp, sp
	mov	[32:bp:-4], r0
	mov	[32:bp:-8], r1
	mov	[32:bp:-12], r2
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
	push	r15
	push	r8
	push	r0
	push	r1
	push	r2
	push	r3
	push	r4
	push	r5
	push	r6
	push	r7
	mov	r11, [32:bp:-20]
	mov	r11, [32:r11]
	mov	r0, r11
	call	puti
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0
	pop	r8
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
L10:
	add	sp, 32
	pop	bp
	ret
	.text
drawchar:
	push	bp
	mov	bp, sp
	mov	[8:bp:-4], r0
	mov	[32:bp:-8], r1
	mov	[32:bp:-12], r2
	mov	[32:bp:-16], r3
	mov	[32:bp:-20], r4
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
L12:
	mov	r11, [32:bp:-36]
	mov	r12, 16
	cmp	r11, r12
	jge	L13
	mov	r11, 7
	mov	[32:bp:-40], r11
L14:
	mov	r11, [32:bp:-40]
	mov	r12, 0
	cmp	r11, r12
	jl	L15
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
	push	r15
	push	r8
	push	r0
	push	r1
	push	r2
	push	r3
	push	r4
	push	r5
	push	r6
	push	r7
	mov	r11, [32:bp:-44]
	mov	r12, 1
	and	r11, r12
	mov	r13, [32:bp:-16]
	mov	r12, r13
	jmp	L17
L16:
	mov	r13, [32:bp:-20]
	mov	r12, r13
L17:
	mov	r2, r12
	mov	r12, [32:bp:-12]
	mov	r1, r12
	mov	r12, [32:bp:-8]
	lea	r13, [32:bp:-8]
	add	[32:r13], 1
	mov	r0, r12
	call	setpixel
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0
	pop	r8
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r12, ax
	mov	r11, [32:bp:-40]
	lea	r12, [32:bp:-40]
	add	[32:r12], -1
	jmp	L14
L15:
	mov	r11, [32:bp:-12]
	lea	r12, [32:bp:-12]
	add	[32:r12], 1
	mov	r11, [32:bp:-24]
	mov	[32:bp:-8], r11
	mov	r11, [32:bp:-36]
	lea	r12, [32:bp:-36]
	add	[32:r12], 1
	jmp	L12
L13:
L11:
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
	push	r15
	push	r8
	push	r0
	push	r1
	push	r2
	push	r3
	push	r4
	push	r5
	push	r6
	push	r7
	mov	r11, 16777215
	mov	r2, r11
	mov	r11, 100
	mov	r1, r11
	mov	r11, 200
	mov	r0, r11
	call	setpixel
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0
	pop	r8
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15
	push	r8
	push	r0
	push	r1
	push	r2
	push	r3
	push	r4
	push	r5
	push	r6
	push	r7
	mov	r11, 760
	mov	r11, [32:r11]
	mov	r0, r11
	call	puti
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0
	pop	r8
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	mov	r11, ax
	mov	r11, 0
	mov	ax, r11
	jmp	L18
L18:
	add	sp, 0
	pop	bp
	ret
.data
bitmap_font:
    #incbin "../audr32/vm/src/bitmap_font.fnt"
.text
