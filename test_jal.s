main:
	addi $a0, $zero, 5
	jal subtract
# some instructions to make sure we're jumping
	add $t0, $zero, $zero
	add $t0, $zero, $zero
	add $t0, $zero, $zero
	add $t0, $zero, $zero
	add $t0, $zero, $zero
.end main

# function that subtracts 1
subtract:
	addi $v0, $a0, -1
# some additional instructions for testing purpose
	add $t0, $zero, $zero
	add $t0, $zero, $zero
	add $t0, $zero, $zero
	add $t0, $zero, $zero
	add $t0, $zero, $zero
# here we should theoretically jump back
	jr $ra
# add a nop because we need it
	add $t0, $zero, $zero
.end subtract
