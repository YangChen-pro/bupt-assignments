.data
arr:
.word 10,9,8,7,6,5,4,3,2,1
len:
.word 10

.text
main:
ADDIU $r1, $r0, arr # 获取数组首地址
ADDIU $r2, $r0, len # 获取len的地址
LW $r2, 0($r2) # 获取数组长度
SLL $r2, $r2, 2 # len<<2
ADD $r2, $r2, $r1 # arr[len]的地址
outer_loop:
ADDI $r2, $r2, -4 # len--
ADDIU $r3, $r1, 0 # 初始 k = 0, 获取arr[k]的地址
BEQ $r1, $r2, exit # 如果arr[i] == arr[len]，则结束
inner_loop:
LW $r4, 0($r3) # 获取arr[k]的值
ADDI $r3, $r3, 4 # k++
LW $r5, 0($r3) # 获取arr[k+1]的值
SLT $r6, $r5, $r4 # arr[k+1] < arr[k] ?
BEQ $r6, $r0, skip # 如果arr[k+1] >= arr[k]，则跳过交换
SW $r5, -4($r3) # 存arr[k+1]到arr[k]
SW $r4, 0($r3) # 存arr[k]到arr[k+1]
skip:
BNE $r3, $r2, inner_loop # 如果k != len，继续循环
BEQ $r0, $r0, outer_loop # 继续循环
exit:
TEQ $r0, $r0 # 结束