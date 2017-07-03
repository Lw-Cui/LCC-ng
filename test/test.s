	.globl foo
	.type  foo, @function
foo:
	pushq  %rbp
	movq   %rsp, %rbp
	subq   %rsp, $32
	# passing a 4 byte(s) -4(%rbp)
	movl   %edi, -4(%rbp)
	# passing b 4 byte(s) -8(%rbp)
	movl   %esi, -8(%rbp)
	# allocate x 4 byte(s) -12(%rbp)
	# allocate y 4 byte(s) -16(%rbp)
	# allocate z 1 byte(s) -17(%rbp)
	# push x
	movl   -12(%rbp), %eax
	# push z
	movl   %eax, -24(%rbp)
	movb   -17(%rbp), %al
	# pop z
	# pop x
	movl   -24(%rbp), %ebx
	# add z and x
	movsbl %al, %eax
	addl   %ebx, %eax
	# push (z add x)
