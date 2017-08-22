.data
shouldben1:	.asciiz "Should be -1, and firstposshift and firstposmask returned: "
shouldbe0:	.asciiz "Should be 0 , and firstposshift and firstposmask returned: "
shouldbe16:	.asciiz "Should be 16, and firstposshift and firstposmask returned: "
shouldbe31:	.asciiz "Should be 31, and firstposshift and firstposmask returned: "

.text
main:
	la	$a0, shouldbe31
	jal	print_str
	lui	$a0, 0x8000	# should be 31
	jal	first1posshift
	move	$a0, $v0
	jal	print_int
	jal	print_space

	lui	$a0, 0x8000
	jal	first1posmask
	move	$a0, $v0
	jal	print_int
	jal	print_newline

	la	$a0, shouldbe16
	jal	print_str
	lui	$a0, 0x0001	# should be 16
	jal	first1posshift
	move	$a0, $v0
	jal	print_int
	jal	print_space

	lui	$a0, 0x0001
	jal	first1posmask
	move	$a0, $v0
	jal	print_int
	jal	print_newline

	la	$a0, shouldbe0
	jal	print_str
	li	$a0, 1		# should be 0
	jal	first1posshift
	move	$a0, $v0
	jal	print_int
	jal	print_space

	li	$a0, 1
	jal	first1posmask
	move	$a0, $v0
	jal	print_int
	jal	print_newline

	la	$a0, shouldben1
	jal	print_str
	move	$a0, $0		# should be -1
	jal	first1posshift
	move	$a0, $v0
	jal	print_int
	jal	print_space

	move	$a0, $0
	jal	first1posmask
	move	$a0, $v0
	jal	print_int
	jal	print_newline

	li	$v0, 10
	syscall

first1posshift:
	### YOUR CODE HERE ###
	addiu	$sp, $sp, -20
	sw	$ra, 0($sp)
	sw	$s0, 4($sp)
	sw	$t0, 8($sp)
	sw	$t1, 12($sp)
	sw	$t2, 16($sp)
	#sw	$v0, 20($sp)
	
	move	$v0, $0
	beq	$a0, $0, return1
	add	$s0, $s0, $0
	addi	$t1, $t1, 32
	add	$t2, $t2, $0
Loop1:
	beq	$t0, $t1, return2
	andi	$t2, $a0, 1
	beq	$t2, $0, Next1
	add	$s0, $t0, 0
	j	Next1
Next1:
	srl	$a0, $a0, 1
	addi	$t0, $t0, 1
	j	Loop1
	
return1:
	addi	$v0, $v0, -1
	
	#lw	$v0, 20($sp)
	lw	$t2, 16($sp)
	lw	$t1, 12($sp)
	lw	$t0, 8($sp)
	lw	$s0, 4($sp)
	lw	$ra, 0($sp)
	addiu	$sp, $sp, 20
	jr	$ra

return2:
	move	$v0, $s0
	
	#lw	$v0, 20($sp)
	lw	$t2, 16($sp)
	lw	$t1, 12($sp)
	lw	$t0, 8($sp)
	lw	$s0, 4($sp)
	lw	$ra, 0($sp)
	addiu	$sp, $sp, 20
	jr	$ra

first1posmask:
	### YOUR CODE HERE ###
	addiu	$sp, $sp, -20
	sw	$ra, 0($sp)
	sw	$s0, 4($sp)
	sw	$t0, 8($sp)
	sw	$t1, 12($sp)
	sw	$t2, 16($sp)
	#sw	$v0, 20($sp)
	
	move	$v0, $0
	beq	$a0, $0, return1b
	add	$s0, $s0, $0
	addi	$t0, $t0, 31
	addi	$t1, $t1, 0
	lui	$t2, 0x8000
	#add	$t3, $t3, $0
Loop1b:
	bne	$t1, $0, return2b
	and	$t1, $a0, $t2
	beq	$t1, $0, Next1b
	move	$s0, $t0 
	j	Loop1b
Next1b:
	srl	$t2, $t2, 1
	addi	$t0, $t0, -1
	j	Loop1b
	
return1b:
	addi	$v0, $v0, -1
	
	#lw	$v0, 20($sp)
	lw	$t2, 16($sp)
	lw	$t1, 12($sp)
	lw	$t0, 8($sp)
	lw	$s0, 4($sp)
	lw	$ra, 0($sp)
	addiu	$sp, $sp, 20
	jr	$ra

return2b:
	move	$v0, $s0
	
	#lw	$v0, 20($sp)
	lw	$t2, 16($sp)
	lw	$t1, 12($sp)
	lw	$t0, 8($sp)
	lw	$s0, 4($sp)
	lw	$ra, 0($sp)
	addiu	$sp, $sp, 20
	jr	$ra

print_int:
	move	$a0, $v0
	li	$v0, 1
	syscall
	jr	$ra

print_str:
	li	$v0, 4
	syscall
	jr	$ra

print_space:
	li	$a0, ' '
	li	$v0, 11
	syscall
	jr	$ra

print_newline:
	li	$a0, '\n'
	li	$v0, 11
	syscall
	jr	$ra
