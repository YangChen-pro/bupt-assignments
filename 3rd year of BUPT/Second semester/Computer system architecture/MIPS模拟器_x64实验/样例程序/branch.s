.text
main:
ADDI  $r2,$r0,1024 # $r2 = 1024
ADD   $r3,$r0,$r0 # $r3 = 0
ADDI  $r4,$r0,8 # $r4 = 8
loop:  
LW    $r1,0($r2) # $r1 = MEM[$r2] = MEM[1024]
ADDI  $r1,$r1,1 # $r1 = $r1 + 1
SW    $r1,0($r2) # MEM[$r2] = $r1, 即 MEM[1024] = MEM[1024] + 1
ADDI  $r3,$r3,4 # $r3 = $r3 + 4
SUB   $r5,$r4,$r3 # $r5 = $r4 - $r3 = 8 - $r3
BGTZ  $r5,loop # if $r5 > 0, goto loop
ADD   $r7,$r0,$r6 # $r7 = $r0 + $r6 = $r6
TEQ   $r0,$r0