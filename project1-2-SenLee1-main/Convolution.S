.import input.S
.data
.globl arr
arr:

.text
main:

    la a2 image_width  
    lw s0 0(a2)            # s0 = m_img
    la a2 image_length  
    lw s1 0(a2)            # s1 = n_img

    la a3 kernel_width  
    lw s2 0(a3)            # s2 = m_ker
    la a3 kernel_length  
    lw s3 0(a3)            # s3 = n_ker

    la t1 image         #  t1 = image[m_img][n_img]
    la t2 kernel           # t2 = kernel[m_ker][n_ker]

    addi t0 x0 0
    neg t3 s2       # t3 = -m_ker
    add s5 s0 t3    # s5 = m_img - m_ker
    addi s5 s5 1    # s5 = (m_img-m_ker + 1)

    neg t3 s3       # t3 = -n_ker
    add s6 s1 t3    # s6 = n_img - n_ker
    addi s6 s6 1    # s6 = (n_img - n_ker + 1)

    mul s4 s5 s6    # s4 = max index = (m_img-m_ker + 1) * (n_img-n_ker + 1)
    # s5 = (m_img-m_ker + 1)
    # s6 = (n_img - n_ker + 1)
    # s4 = max index = (m_img-m_ker + 1) * (n_img-n_ker + 1)

    add t6 x0 x0   # a call for each line end "\n"
    li t0 0
    add a4 x0 x0  # number of inserted item
    jal ra Loop
    li a0 10
    ecall


Loop:
    slli t3 t0 2        #
    bge a4 s4 exit      # if t3 >= max index: exit
    add t4 t1 t3        # t4 = image[][] the starting point 
    sw t0 0(sp)
    sw t3 4(sp)
    sw t6 8(sp)
    sw t4 12(sp)
    sw t2 16(sp)

    add t0 x0 x0
    add a1 x0 x0
    add a2 x0 x0
    add a3 x0 x0    

    sw ra 20(sp)
    jal ra loop_each
    lw ra 20(sp)
    addi a4 a4 1
    
    lw t0 0(sp)
    lw t3 4(sp)
    lw t6 8(sp)
    lw t4 12(sp)
    lw t2 16(sp)
    addi a0 x0 11 # print_character
    addi a1 x0 32 #" " //////////////////////////////////////
    ecall
    addi t6 t6 1    # number of inserted for current line
    beq t6 s6 line_end ###################
    
    addi t0 t0 1    
    j Loop
line_end:
    add t6 x0 x0
    add t0 t0 s3        # come to end, step the back of one line to the first of next line
    
    addi a0 x0 11        # print_character
    addi a1 x0 10       # "\n"
    ecall
    j Loop
loop_each:
    beq t0 s3 addline  # s3: n_ker  t0:number we have added at current line
    beq a3 s2 exit_each     # s2 : m_ker
    
    lw t3 0(t4)        # t3 = current image
    lw t5 0(t2)        # t5 = current kernel
    mul t6 t3 t5       # t6 = t3 * t5
    
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

exit:
    jr ra
