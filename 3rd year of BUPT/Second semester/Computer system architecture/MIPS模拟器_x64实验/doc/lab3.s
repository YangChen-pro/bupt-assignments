.data
    vectorA: .word 1, 2, 3, 4, 5, 6, 7, 8, 9, 10  # 向量A
    vectorB: .word 10, 9, 8, 7, 6, 5, 4, 3, 2, 1  # 向量B
    size: .word 10  # 向量的维度
    result: .word 0  # 存储点积结果
.text
main:
    ADDIU $r1, $r0, vectorA  # 加载向量A的地址
    ADDIU $r2, $r0, vectorB  # 加载向量B的地址
    ADDIU $r3, $r0, result  # 加载结果的地址
    ADDIU $r4, $r0, size # 加载 size 的地址
    LW $r4, 0($r4)  # 加载向量的维度
    ADDIU $r5, $r0, 0  # 初始化索引i为0
    ADDIU $r6, $r0, 0  # 初始化结果为0

loop:
    LW $r7, 0($r1)  # 加载向量A的第i个元素
    LW $r8, 0($r2)  # 加载向量B的第i个元素

    MUL $r9, $r7, $r8  # 计算A[i]*B[i]

    ADDI $r1, $r1, 4  # 移动到向量A的下一个元素
    ADDI $r2, $r2, 4  # 移动到向量B的下一个元素
    ADDI $r5, $r5, 1  # 索引i加1
    
    ADD $r6, $r6, $r9  # 累加到结果中

    BNE $r5, $r4, loop  # 如果i不等于向量的维度，继续循环

    SW $r6, 0($r3)  # 将结果存储到内存中

    # 结束程序
    TEQ $r0, $r0