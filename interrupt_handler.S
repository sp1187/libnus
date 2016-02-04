.extern nus_scheduler
.extern _nus_handle_interrupt
.extern _nus_thread_reschedule

.global interrupt_handler
.type interrupt_handler, @function
.text

interrupt_handler:
	.set noreorder

	addiu $sp, $sp, -376

	#save registers

	.set noat

	sd $at, 0($sp)
	sd $v0, 8($sp)
	sd $v1, 16($sp)
	sd $a0, 24($sp)
	sd $a1, 32($sp)
	sd $a2, 40($sp)
	sd $a3, 48($sp)
	sd $t0, 56($sp)
	sd $t1, 64($sp)
	sd $t2, 72($sp)
	sd $t3, 80($sp)
	sd $t4, 88($sp)
	sd $t5, 96($sp)
	sd $t6, 104($sp)
	sd $t7, 112($sp)
	sd $t8, 120($sp)
	sd $t9, 128($sp)
	sd $ra, 136($sp)

	.set at

	mfhi $k0
	mflo $k1
	sd $k0, 144($sp)
	sd $k1, 152($sp)

	dmfc0 $t0, $14 #EPC
	sd $t0, 160($sp)

	sdc1 $f0, 168($sp)
	sdc1 $f2, 176($sp)
	sdc1 $f4, 184($sp)
	sdc1 $f6, 192($sp)
	sdc1 $f8, 200($sp)
	sdc1 $f10, 208($sp)
	sdc1 $f12, 216($sp)
	sdc1 $f14, 224($sp)
	sdc1 $f16, 232($sp)
	sdc1 $f18, 240($sp)
	sdc1 $f1, 248($sp)
	sdc1 $f3, 256($sp)
	sdc1 $f5, 264($sp)
	sdc1 $f7, 272($sp)
	sdc1 $f9, 280($sp)
	sdc1 $f11, 288($sp)
	sdc1 $f13, 296($sp)
	sdc1 $f15, 304($sp)
	sdc1 $f17, 312($sp)
	sdc1 $f19, 320($sp)
	sdc1 $f21, 328($sp)
	sdc1 $f23, 336($sp)
	sdc1 $f25, 344($sp)
	sdc1 $f27, 352($sp)
	sdc1 $f29, 360($sp)
	sdc1 $f31, 368($sp)
	
	#run interrupt routine

	move $k0, $sp

	la $k1, nus_scheduler
	lw $sp, 8($k1) #irq_sp

	jal _nus_handle_interrupt
	nop

	move $sp, $k0

	#perform a thread reschedule

	jal _nus_thread_reschedule
	nop

	#load registers

	.set noat

	ld $at, 0($sp)
	ld $v0, 8($sp)
	ld $v1, 16($sp)
	ld $a0, 24($sp)
	ld $a1, 32($sp)
	ld $a2, 40($sp)
	ld $a3, 48($sp)
	ld $t0, 56($sp)
	ld $t1, 64($sp)
	ld $t2, 72($sp)
	ld $t3, 80($sp)
	ld $t4, 88($sp)
	ld $t5, 96($sp)
	ld $t6, 104($sp)
	ld $t7, 112($sp)
	ld $t8, 120($sp)
	ld $t9, 128($sp)
	ld $ra, 136($sp)

	ld $k0, 144($sp)
	ld $k1, 152($sp)
	mthi $k0
	mtlo $k1

	ld $k0, 160($sp)
	dmtc0 $k0, $14 #EPC

	ldc1 $f0, 168($sp)
	ldc1 $f2, 176($sp)
	ldc1 $f4, 184($sp)
	ldc1 $f6, 192($sp)
	ldc1 $f8, 200($sp)
	ldc1 $f10, 208($sp)
	ldc1 $f12, 216($sp)
	ldc1 $f14, 224($sp)
	ldc1 $f16, 232($sp)
	ldc1 $f18, 240($sp)
	ldc1 $f1, 248($sp)
	ldc1 $f3, 256($sp)
	ldc1 $f5, 264($sp)
	ldc1 $f7, 272($sp)
	ldc1 $f9, 280($sp)
	ldc1 $f11, 288($sp)
	ldc1 $f13, 296($sp)
	ldc1 $f15, 304($sp)
	ldc1 $f17, 312($sp)
	ldc1 $f19, 320($sp)
	ldc1 $f21, 328($sp)
	ldc1 $f23, 336($sp)
	ldc1 $f25, 344($sp)
	ldc1 $f27, 352($sp)
	ldc1 $f29, 360($sp)
	ldc1 $f31, 368($sp)

	.set at

	nop

	addiu $sp, $sp, 376
	eret
	nop
