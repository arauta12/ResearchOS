#define FUNC(name) .globl name; .type name,@function; name
	.file	"syms.S"
	.data
dvar1:
	.quad 1488
dvar2:
	.quad 42

	.bss
bvar1:
	.space	4
bvar2:
	.space	4

	.text
	.extern	extFunct
FUNC(_start):
	movq	dvar1, %rax
	movq	dvar2, %rax
	movq	bvar1, %rax
	movq	bvar2, %rax
1:
	call	extFunct
	jmp	1
localLbl:
	.long 0
