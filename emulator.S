	.file	"emulator.cpp"
	.text
	.globl	ram
	.bss
	.align 8
	.type	ram, @object
	.size	ram, 8
ram:
	.zero	8
	.globl	cores
	.align 8
	.type	cores, @object
	.size	cores, 8
cores:
	.zero	8
	.globl	bios
	.align 8
	.type	bios, @object
	.size	bios, 8
bios:
	.zero	8
	.globl	bios_name
	.section	.rodata
.LC0:
	.string	"std_bios.bin"
	.section	.data.rel.local,"aw"
	.align 8
	.type	bios_name, @object
	.size	bios_name, 8
bios_name:
	.quad	.LC0
	.globl	ram_size
	.data
	.align 8
	.type	ram_size, @object
	.size	ram_size, 8
ram_size:
	.quad	1048576
	.globl	cores_count
	.type	cores_count, @object
	.size	cores_count, 1
cores_count:
	.byte	1
	.section	.rodata
.LC1:
	.string	"rb"
.LC2:
	.string	"File %s not found!\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB1893:
	.cfi_startproc
	.cfi_personality 0x9b,DW.ref.__gxx_personality_v0
	.cfi_lsda 0x1b,.LLSDA1893
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$32, %rsp
	.cfi_offset 14, -24
	.cfi_offset 13, -32
	.cfi_offset 12, -40
	.cfi_offset 3, -48
	movq	ram_size(%rip), %rax
	movq	%rax, %rdi
.LEHB0:
	call	_Znam@PLT
	movq	%rax, ram(%rip)
	movzbl	cores_count(%rip), %eax
	movzbl	%al, %ebx
	movabsq	$11085783698142759, %rax
	cmpq	%rbx, %rax
	jb	.L2
	movq	%rbx, %rax
	addq	%rax, %rax
	addq	%rbx, %rax
	salq	$2, %rax
	addq	%rbx, %rax
	salq	$6, %rax
	movq	%rax, %rdi
	call	_Znam@PLT
	movq	%rax, %r12
	movl	$1, %r14d
	movq	%r12, %r13
	leaq	-1(%rbx), %rax
	movq	%rax, %rbx
	jmp	.L4
.L2:
	call	__cxa_throw_bad_array_new_length@PLT
.LEHE0:
.L5:
	movq	%r13, %rdi
.LEHB1:
	call	_ZN4CoreC1Ev@PLT
.LEHE1:
	subq	$1, %rbx
	addq	$832, %r13
.L4:
	testq	%rbx, %rbx
	jns	.L5
	movq	%r12, cores(%rip)
	movl	$0, -52(%rbp)
	jmp	.L6
.L7:
	movq	cores(%rip), %rcx
	movl	-52(%rbp), %eax
	movslq	%eax, %rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$6, %rax
	leaq	(%rcx,%rax), %rdx
	movl	-52(%rbp), %eax
	movzbl	%al, %eax
	movl	%eax, %esi
	movq	%rdx, %rdi
.LEHB2:
	call	_ZN4Core4initEh@PLT
	addl	$1, -52(%rbp)
.L6:
	movzbl	cores_count(%rip), %eax
	movzbl	%al, %eax
	cmpl	%eax, -52(%rbp)
	jl	.L7
	movq	bios_name(%rip), %rax
	leaq	.LC1(%rip), %rdx
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	fopen@PLT
	movq	%rax, -40(%rbp)
	cmpq	$0, -40(%rbp)
	jne	.L8
	movq	bios_name(%rip), %rax
	movq	%rax, %rsi
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$2, %eax
	jmp	.L17
.L8:
	movq	-40(%rbp), %rax
	movl	$2, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	fseek@PLT
	movq	-40(%rbp), %rax
	movq	%rax, %rdi
	call	ftell@PLT
	movl	%eax, -44(%rbp)
	movq	-40(%rbp), %rax
	movq	%rax, %rdi
	call	rewind@PLT
	movl	-44(%rbp), %eax
	movq	%rax, %rdi
	call	_Znam@PLT
	movq	%rax, bios(%rip)
	movl	-44(%rbp), %edx
	movq	bios(%rip), %rax
	movq	-40(%rbp), %rcx
	movl	$1, %esi
	movq	%rax, %rdi
	call	fread@PLT
	movq	-40(%rbp), %rax
	movq	%rax, %rdi
	call	fclose@PLT
	movl	$0, -48(%rbp)
	jmp	.L10
.L11:
	movq	bios(%rip), %rdx
	movl	-48(%rbp), %eax
	cltq
	addq	%rdx, %rax
	movq	ram(%rip), %rdx
	movl	-48(%rbp), %ecx
	movslq	%ecx, %rcx
	addq	$1024, %rcx
	addq	%rcx, %rdx
	movzbl	(%rax), %eax
	movb	%al, (%rdx)
	addl	$1, -48(%rbp)
.L10:
	movl	-48(%rbp), %eax
	cmpl	-44(%rbp), %eax
	jb	.L11
	movq	cores(%rip), %rax
	movl	$1, %edx
	movl	$1, %esi
	movq	%rax, %rdi
	call	_ZN4Core8set_flagEhh@PLT
	movq	cores(%rip), %rax
	movq	$1024, 800(%rax)
	jmp	.L12
.L13:
	movq	cores(%rip), %rax
	movq	%rax, %rdi
	call	_ZN4Core4stepEv@PLT
	movq	stdin(%rip), %rax
	movq	%rax, %rdi
	call	getc@PLT
.L12:
	movq	cores(%rip), %rax
	movl	$1, %esi
	movq	%rax, %rdi
	call	_ZN4Core8get_flagEh@PLT
	testb	%al, %al
	setne	%al
	testb	%al, %al
	jne	.L13
	movl	$0, %eax
	jmp	.L17
.L16:
	movq	%rax, %rbx
	testb	%r14b, %r14b
	je	.L15
	movq	%r12, %rdi
	call	_ZdaPv@PLT
.L15:
	movq	%rbx, %rax
	movq	%rax, %rdi
	call	_Unwind_Resume@PLT
.LEHE2:
.L17:
	addq	$32, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1893:
	.globl	__gxx_personality_v0
	.section	.gcc_except_table,"a",@progbits
.LLSDA1893:
	.byte	0xff
	.byte	0xff
	.byte	0x1
	.uleb128 .LLSDACSE1893-.LLSDACSB1893
.LLSDACSB1893:
	.uleb128 .LEHB0-.LFB1893
	.uleb128 .LEHE0-.LEHB0
	.uleb128 0
	.uleb128 0
	.uleb128 .LEHB1-.LFB1893
	.uleb128 .LEHE1-.LEHB1
	.uleb128 .L16-.LFB1893
	.uleb128 0
	.uleb128 .LEHB2-.LFB1893
	.uleb128 .LEHE2-.LEHB2
	.uleb128 0
	.uleb128 0
.LLSDACSE1893:
	.text
	.size	main, .-main
	.hidden	DW.ref.__gxx_personality_v0
	.weak	DW.ref.__gxx_personality_v0
	.section	.data.rel.local.DW.ref.__gxx_personality_v0,"awG",@progbits,DW.ref.__gxx_personality_v0,comdat
	.align 8
	.type	DW.ref.__gxx_personality_v0, @object
	.size	DW.ref.__gxx_personality_v0, 8
DW.ref.__gxx_personality_v0:
	.quad	__gxx_personality_v0
	.ident	"GCC: (GNU) 12.2.1 20230111"
	.section	.note.GNU-stack,"",@progbits
