
.text
.global _nus_context_switch
.type _nus_context_switch, @function

_nus_context_switch:
	.set noreorder
	.set noat

	addiu $sp, $sp, -136

	sd $s0, 0($sp)
	sd $s1, 8($sp)
	sd $s2, 16($sp)
	sd $s3, 24($sp)
	sd $s4, 32($sp)
	sd $s5, 40($sp)
	sd $s6, 48($sp)
	sd $s7, 56($sp)
	sd $s8, 64($sp)
	sd $ra, 72($sp)
	sdc1 $f20, 80($sp)
	sdc1 $f22, 88($sp)
	sdc1 $f24, 96($sp)
	sdc1 $f26, 104($sp)
	sdc1 $f28, 112($sp)
	sdc1 $f30, 120($sp)
	mfc0 $t0, $12
	sw $t0, 128($sp)
	cfc1 $t0, $31
	sw $t0, 132($sp)

	sw $sp, 0($a1)
	lw $sp, 0($a0)

	ld $s0, 0($sp)
	ld $s1, 8($sp)
	ld $s2, 16($sp)
	ld $s3, 24($sp)
	ld $s4, 32($sp)
	ld $s5, 40($sp)
	ld $s6, 48($sp)
	ld $s7, 56($sp)
	ld $s8, 64($sp)
	ld $ra, 72($sp)
	ldc1 $f20, 80($sp)
	ldc1 $f22, 88($sp)
	ldc1 $f24, 96($sp)
	ldc1 $f26, 104($sp)
	ldc1 $f28, 112($sp)
	ldc1 $f30, 120($sp)
	lw $t0, 128($sp)
	mtc0 $t0, $12
	lw $t0, 132($sp)
	ctc1 $t0, $31

	jr $ra
	addiu $sp, $sp, 136


