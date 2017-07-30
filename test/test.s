	.globl foo
	.type  foo, @function
foo:
	pushq  %rbp
	movq   %rsp, %rbp
	subq   $32, %rsp
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
	movl   %eax, %eax
	# pop z
	movb   -18(%rbp), %bl
	# z add x
	movsbl %bl, %ebx
	addl   %ebx, %eax
	# push (z add x)
	# push y
	movl   %eax, -24(%rbp)
	movl   -16(%rbp), %eax
	# push z
	movl   %eax, -28(%rbp)
	movb   -17(%rbp), %al
	# pop z
	movb   %al, %al
	# pop y
	movl   -28(%rbp), %ebx
	# y imul z
	movsbl %al, %eax
	imull  %ebx
	# push (y imul z)
	# pop (y imul z)
	movl   %eax, %eax
	# pop (z add x)
	movl   -24(%rbp), %ebx
	# (z add x) sub (y imul z)
	subl   %ebx, %eax
	# push ((z add x) sub (y imul z))
	# assign ((z add x) sub (y imul z)) to y
	movl   %eax, -16(%rbp)
	# assign ((z add x) sub (y imul z)) to x
	movl   %eax, -12(%rbp)
	# ------ EOF ------
	# push z
	movb   -17(%rbp), %al
	# push x
	movb   %al, -18(%rbp)
	movl   -12(%rbp), %eax
	# pop x
	movl   %eax, %eax
	# pop z
	movb   -18(%rbp), %bl
	# z add x
	movsbl %bl, %ebx
	addl   %ebx, %eax
	# push (z add x)
	# assign (z add x) to y
	movl   %eax, -16(%rbp)
	# assign (z add x) to x
	movl   %eax, -12(%rbp)
	# ------ EOF ------
	# push x
	movl   -12(%rbp), %eax
	# push z
	movl   %eax, -24(%rbp)
	movb   -17(%rbp), %al
	# pop z
	movb   %al, %bl
	movsbl %bl, %ebx
	# pop x
	movl   -24(%rbp), %eax
	# x idiv z
	idivl  %ebx
	# push (x idiv z)
	# assign (x idiv z) to z
	movb   %al, -17(%rbp)
	# ------ EOF ------
