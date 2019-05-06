main:
	addi $a0, $zero, 6
	jal fibonacci
.end main

fibonacci:
	addi $sp, $sp, -12
	sw $ra, ($sp)

	bne $zero, $a0, nu_zero
	or $v0, $zero, $zero
	j gata
nu_zero:
	ori $v0, $zero, 1
	beq $v0, $a0, gata
recursie:
	addi $a0, $a0, -1
	sw $a0, 4($sp)
	jal fibonacci
	add $t0, $zero, $v0

	lw $a0, 4($sp)
	sw $t0, 8($sp)
	addi $a0, $a0, -1
	jal fibonacci

	lw $t0, 8($sp)
	add $v0, $v0, $t0
gata:
	lw $ra, ($sp)
	addi $sp, $sp, 12
	jr $ra
.end fibonacci

