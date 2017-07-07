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
	# push z
	movb   -17(%rbp), %al
	# push x
	movb   %al, -18(%rbp)
	movl   -12(%rbp), %eax
	# pop x
	# pop z
	movb   -18(%rbp), %bl
	# z add x
	movsbl %bl, %ebx
	addl   %ebx, %eax
	# push (z add x)
	# push y
	movl   %eax, -24(%rbp)
	movl   -16(%rbp), %eax
	# pop y
	# pop (z add x)
	movl   -24(%rbp), %ebx
	# (z add x) sub y
	subl   %ebx, %eax
	# push ((z add x) sub y)
	# assign ((z add x) sub y) to y
	movl   %eax, -16(%rbp)
	# assign ((z add x) sub y) to x
	movl   %eax, -12(%rbp)
