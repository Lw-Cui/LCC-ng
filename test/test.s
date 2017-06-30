	.globl foo
	.type  foo, @function
foo:
	pushq  %rbp
	movq   %rsp, %rbp
	subq   %rsp, 16
	# passing a 4 byte(s) -4(%rbp)
	movl   %edi, -4(%rbp)
	# passing b 4 byte(s) -8(%rbp)
	movl   %esi, -8(%rbp)
	# allocate x 4 byte(s) -12(%rbp)
	# allocate y 4 byte(s) -16(%rbp)
