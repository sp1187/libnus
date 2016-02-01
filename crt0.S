# From tj90241/n64chain with some minor changes.

.extern interrupt_handler

.section .ipl_handoff, "ax", @progbits

.global __ipl_handoff
.type	__ipl_handoff, @function
__ipl_handoff:

.align 5

.set noat
.set noreorder

	# Prevent the PIF from resetting the console.
	addiu $a0, $zero, 8
	lui $at, 0xBFC0
	sw $a0, 0x7FC($at)

	# Setup a stack at the top of cached RAM.
	lui $at, 0x8000
	lw $at, 0x318($at)
	lui $sp, 0x8000
	daddu $sp, $sp, $at

	# Set the global pointer reference value.
	la $gp, _gp

	# Set CP0/1 usable, disable interrupts
	# and enable 64-bit modes for k/s/u.
	lui $v0, 0x3000
	ori $v0, $v0, 0x00E0
	mtc0 $v0, $12

	#copy interrupt vector
	la $t0, interrupt_vector
	la $t1, 0xa0000000

	ld $t2, 0($t0)
	sd $t2, 0x180($t1)
	ld $t2, 8($t0)
	sd $t2, 0x188($t1)
	
	cache 16, 0x180($t1)
	
	# Clear out cause bits and jump to main.
	jal main
	mtc0 $zero, $13

	
  # We shouldn't get here, but just in case...
__ipl_handoff_spin:
	j __ipl_handoff_spin
	nop

interrupt_vector:
	la $k0, interrupt_handler
	jr $k0
	nop

.set reorder
.set at

.size __ipl_handoff,.-__ipl_handoff

