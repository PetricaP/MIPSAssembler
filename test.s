# n
addi $t0, $t0, 5
# index
add $t2, $zero, $zero
# sum
add $t1, $zero, $zero
loop:
addi $t2, $t2, 1
add $t1, $t1, $t2
bne $t2, $t0, loop
# Can't end program in BNE or I get undefined behaviour
#nop
add $zero, $zero, $zero
#nop
add $zero, $zero, $zero

