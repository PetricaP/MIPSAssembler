main:
	addi $a0, $zero, 6 #1
	jal fibonacci #2
.end main

fibonacci:
	addi $sp, $sp, -12 #3
	sw $ra, ($sp) #4

	bne $zero, $a0, nu_zero #5
	or $v0, $zero, $zero #6
	j gata #7
nu_zero:
	addi $v0, $zero, 1 #8
	beq $v0, $a0, gata #9
recursie:
	addi $a0, $a0, -1 #10
	sw $a0, 4($sp) #11
	jal fibonacci #12
	add $t0, $zero, $v0 #13

	lw $a0, 4($sp) #14
	sw $t0, 8($sp) #15
	addi $a0, $a0, -1 #16
	jal fibonacci #17

	lw $t0, 8($sp) #18
	add $v0, $v0, $t0 #19
gata:
	lw $ra, ($sp) #20
	addi $sp, $sp, 12 #21
	jr $ra #22
.end fibonacci

