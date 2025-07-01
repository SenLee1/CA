.data
# length of image matrix
.globl image_length
image_length:
	.word 5
# width of image matrix
.globl image_width
image_width:
	.word 5
# image matrix
# -2    12   14   28   -13
#  1    11   3   -26    20
# -8    30   5    29   -24
#  27   4   -29   25   -13
# -27  -1   -21   17    5
.globl image
image:
	.word -2 12 14 28 -13 1 11 3 -26 20 -8 30 5 29 -24 27 4 -29 25 -13 -27 -1 -21 17 5 
# length of kernel matrix
.globl kernel_length
kernel_length:
	.word 2
# width of kernel matrix
.globl kernel_width
kernel_width:
	.word 2
# kernel matrix
# 0 3
# 0 6
.globl kernel
kernel:
	.word 0 3 0 6 