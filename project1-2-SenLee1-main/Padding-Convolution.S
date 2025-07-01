.import input.S

.text
main:
    la a2 image_width  
    lw s0 0(a2)            # s0 = m_img
    la a2 image_length  
    lw s1 0(a2)            # s1 = n_img
              

    la a3 kernel_width  
    lw s2 0(a3)            # s2 = n_ker

    add s7 x0 x0
    addi s7 s2 -1          
    add t0 s7 s0
    add t1 s7 s1
    mul a1 t0 t1
    
    slli a1 a1 2
    li a0 0x3cc
    li a6 1
    ecall
    add t3 a0 x0
    la t1 image         #  t1 = image[m_img][n_img]0
    la t2 kernel           # t2 = kernel[m_ker][n_ker]

    add s6 s2 x0    
    neg s6 s6
    add s6 s1 s6    # s6 = n_img - n_ker
    
    addi t0 x0 2
    
    div s7 s7 t0           # s7 = m_ker-1 /2
    addi t0 x0 0

    add s6 x0 s1
    mul s4 s0 s1    # s4 = max index 

    # s6 = (n_img-n_ker + 1 + m_ker-1)
    # s4 = max index 
    li t0 0
    li a0 1       # a0 : curret line (1 first)
    add t4 x0 x0
    add t5 x0 x0
    # expand t1 and store it in t3 (arr)

    jal ra Expand_matrix
    li t0 0
    add t1 x0 x0
    add a4 x0 x0  # number of inserted item
    add t6 x0 x0   # a call for each line end "\n"

    jal ra Loop
    li a0 0x3cc
    add a1 t3 x0
    li a6 4
    ecall
    
    li a0 10
    ecall

Loop:
    slli t1 t0 2        #
    bge a4 s4 exit0      # if t3 >= max index: exit
    add t4 t3 t1        # t4 = image[][] the starting point 
    addi sp sp -24
    sw t0 0(sp)
    sw t1 4(sp)
    sw t6 8(sp)
    sw t4 12(sp)
    sw t2 16(sp)

    add t0 x0 x0
    add a1 x0 x0
    add a2 x0 x0
    add a3 x0 x0    

    sw ra 20(sp)
    add t0 x0 x0
    jal ra loop_each
    lw ra 20(sp)
    addi a4 a4 1
    
    lw t0 0(sp)
    lw t1 4(sp)
    lw t6 8(sp)
    lw t4 12(sp)
    lw t2 16(sp)
    addi sp sp 24
    addi a0 x0 11 # print_character
    addi a1 x0 32 #" " //////////////////////////////////////
    ecall
    addi t6 t6 1    # number of inserted for current line
    beq t6 s1 line_end ###################
    
    addi t0 t0 1    
    j Loop
line_end:
    add t6 x0 x0
    add t0 t0 s2        # come to end, step the back of one line to the first of next line
    
    addi a0 x0 11        # print_character
    addi a1 x0 10       # "\n"
    ecall
    j Loop
loop_each:
    beq t0 s2 addline  # s2: n_ker  t0:number we have added at current line
    beq a3 s2 exit_each     # s2 : m_ker
    
    lw t1 0(t4)        # t3 = current image
    lw t5 0(t2)        # t5 = current kernel
    mul t6 t1 t5       # t6 = t3 * t5
    
    add a1 a1 t6       # result for one location
    addi t0 t0 1        
    addi t4 t4 4        # kernel 
    addi t2 t2 4        # image next 

    j loop_each
addline: 
    addi a3 a3 1        # a3: number of lines we have finished
    li t0 4
    mul a2 s6 t0        # a2: the step (*4) that we should move for image
    add t4 t4 a2        # t4: next arr head of image
    addi t4 t4 -4       # to cancel line 123 "addi t4 t4 4"
    add t0 x0 x0        # when coming to a newline, renew t0
    j loop_each
exit_each:
    addi a0 x0 1        # print_int ecall in a1
    ecall
    jr ra             
















exit0:
    jr ra
Expand_matrix:
    li t4 0
    beq s7 t4 no_change
    addi sp sp -12
    sw  t3 0(sp)
    sw  s7 4(sp)
    sw ra 8(sp)
    add t4 x0 x0
    add t4 s7 s7
    add t4 t4 s1
    mul t4 t4 s7  # t4 = # of top and end lines' numbers
    add t5 x0 t4  # t5 = t4
    mul t5 t5 s0  # t5 = # of middle lines' numbers 

    jal ra top
    li a0 0

    jal ra mid

    jal ra top
    lw t3 0(sp)
    lw s7 4(sp)
    lw ra 8(sp)
    addi sp sp 12
    jr ra
no_change:
    la t3 image
    jr ra
top:
    beq t0 t4 exit  # topline finished
    sw x0 0(t3)
    addi t3 t3 4
    addi a7 a7 4
    addi t0 t0 1
    j top
mid:
    beq a0 s0 exit
    addi sp sp -4
    sw ra 0(sp)
    jal ra mid_onelie
    lw ra 0(sp)
    addi sp sp 4
    addi a0 a0 1
    j mid
mid_onelie:
    addi sp sp -4
    sw ra 0(sp)
    jal ra side
    jal ra middle
    jal ra side
    lw ra 0(sp)
    addi sp sp 4
    jr ra
side:
    sw x0 0(t3)
    addi t0 t0 1
    addi t3 t3 4
    addi a7 a7 4
    beq t0 s7 exit
    j side
middle:
    lw t5 0(t1)
    sw t5 0(t3)
    addi t3 t3 4
    addi a7 a7 4
    addi t1 t1 4
    addi t0 t0 1
    beq t0 s1 exit
    j middle
exit:
    li t0 0
    jr ra