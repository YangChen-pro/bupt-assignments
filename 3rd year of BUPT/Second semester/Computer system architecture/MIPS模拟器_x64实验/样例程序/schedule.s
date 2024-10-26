.text
main:
ADDIU  $r1,$r0,A # r1 = A = 56
LW     $r2,0($r1) # r2 = A[0] = 4
ADD    $r4,$r0,$r2 
SW     $r4,0($r1) # A[0] = r4 = 0
LW     $r6,4($r1) # r6 = A[1] = 6
ADD    $r8,$r6,$r1 # r8 = 6 + 56 = 62
MUL    $r12,$r10,$r1 # r12 = 0 * 56 = 0
ADD    $r16,$r12,$r1 # r16 = 0 + 56 = 56
ADD    $r18,$r16,$r1 # r18 = 56 + 56 = 112
SW     $r18,16($r1) # A[4] = r18 = 112
LW     $r20,8($r1) # r20 = A[2] = 8
MUL    $r22,$r20,$r14 # r22 = 8 * 0 = 0
MUL    $r24,$r26,$r14 # r24 = 0 * 0 = 0
TEQ $r0,$r0

.data
A: 
.word 4,6,8
