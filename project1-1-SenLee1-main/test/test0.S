addi a0 a1 x1
srai a0 a1 -1
slli a0 a1 32
addi a0 a0 2048
addi a0 a0 -2049
lb a0 2048(a1)
sw a0 114514(rd)
addi x32 x1 10
sb a0 2077(a0)
beq a0 a1 4099
lw a0 0(x33)
addi a0 a1 two
lw a0 offset(a1)
addi a0 a1 2048
addi a0 a1 -2049
moveto a0 a1
addi a0 a0 a1
lw zr 0(a1)
addi a0 a1 30000