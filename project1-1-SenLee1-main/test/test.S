add a0 a0 a1
sltiu a2 t0 9
add x0 x1 x2
mul x3 x4 x5
sub x6 x7 x8
sll x9 x10 x11
mulh x12 x13 x14
slt x15 x16 x17
sltu x18 x19 x20
xor x21 x22 x23
div x24 x25 x26
srl x27 x28 x29
sra x30 x31 zero
or ra sp gp
rem tp t0 t1
and t2 s0 s1
lb a0 13(a1)
lh a2 44(a3)
lw a4 -4(a5)
lbu a6 8(a7)
lhu s2 0(s3)
addi s4 s5 -100
slli s6 s7 4
slti s8 s9 12
sltiu s10 s11 23
xori t3 t4 995
srli t5 t6 3
srai a0 t2 4
ori zero x3 465
andi x9 x22 78
jalr x5 x17 4
ecall
sb x4 0(ra)
sh sp -2(s0)
sw gp -12(a4)
bne a5 t4 -68
blt t1 s0 24
bge s8 s3 0
bltu x29 x23 40
bgeu x14 x8 12
auipc ra 8888
lui gp 2024
li a0 100
li a0 1000000
jr ra
beqz a0 100
bnez t0 -100
mv x9 x10
jal x0 16
j 16