import re

path = r"附件4.0-1背包问题输入数据-2023.txt"


def read_file(group):
    """
    读取文本文件内容
    :param group: 选择要读取的组, 1表示第一组, 2表示第二组
    :return:  背包容量, 物品重量列表, 物品价值列表
    """
    # 读取文本文件内容
    with open(path, "r") as file:
        content = file.read()

    # 根据参数group选择要读取的组
    if group == 1:
        start_index = content.index("第一组") + len("第一组") + 1
        end_index = content.index("第二组")
    elif group == 2:
        start_index = content.index("第二组") + len("第二组") + 1
        end_index = len(content)
    else:
        raise ValueError("Invalid group number. Must be 1 or 2.")

    # 提取容量、重量和价值数据
    group_data = content[start_index:end_index].strip().split("\n\n")

    # 提取容量
    re_pattern = r"(\d+)"  # 正则表达式, 匹配数字
    capacity = int(re.findall(re_pattern, group_data[0])[0])

    # 提取重量和价值列表
    weights = re.findall(re_pattern, group_data[1])
    weights = [int(weight) for weight in weights]
    values = re.findall(re_pattern, group_data[2])
    values = [int(value) for value in values]
    return capacity, weights, values


def zero_one_knapsack(capacity, weights, values):
    """
    0-1背包问题
    :param capacity: 背包容量
    :param weights: 物品重量列表
    :param values: 物品价值列表
    :return: 最大价值和最优解, 最优解为一个列表, x[i] = 1表示第 i + 1 个物品放入背包中
    """
    # 初始化二维数组
    n = len(weights)  # 物品数量
    m = capacity  # 背包容量
    f = [
        [0 for _ in range(m + 1)] for _ in range(n + 1)
    ]  # f[i][j]表示前i个物品放入容量为j的背包中的最大价值

    # 动态规划
    for i in range(1, n + 1):
        for j in range(1, m + 1):
            if weights[i - 1] <= j:
                f[i][j] = max(f[i - 1][j], f[i - 1][j - weights[i - 1]] + values[i - 1])
            else:
                f[i][j] = f[i - 1][j]

    # 逆推出最优解, x[i] = 1表示第 i + 1 个物品放入背包中
    x = [0 for _ in range(n)]
    j = m
    for i in range(n, 0, -1):
        if f[i][j] > f[i - 1][j]:
            x[i - 1] = 1
            j -= weights[i - 1]
    return f[n][m], x


def solve(num):
    """
    解决背包问题
    :param num: 选择要解决的问题, 1表示第一组, 2表示第二组
    :return: None
    """
    n, w, v = read_file(num)
    max_value, x = zero_one_knapsack(n, w, v)
    print(f"第{num}组, 最大价值为: ", max_value)
    sum_weight, check = 0, 0
    for i in range(len(x)):
        if x[i] == 1:
            sum_weight += w[i]
            check += v[i]
            print("第{}个物品放入背包中，重量为{}, 价值为{}".format(i + 1, w[i], v[i]))
    print("背包总重量为: ", sum_weight)

    # 检查最优解是否正确
    if max_value == check:
        print("最优解正确\n")
    else:
        print("最优解错误\n")


def zero_one_knapsack_saving_space(capacity, weights, values):
    """
    0-1背包问题, 空间复杂度为O(m)
    :param capacity: 背包容量
    :param weights: 物品重量列表
    :param values: 物品价值列表
    :return: 最大价值和最优解, 最优解为一个列表, x[i] = 1表示第 i + 1 个物品放入背包中
    """
    # 初始化一维数组
    n = len(weights)  # 物品数量
    m = capacity  # 背包容量
    f = [0 for _ in range(m + 1)]  # f[j]表示容量为j的背包中的最大价值

    # 动态规划
    for i in range(1, n + 1):
        for j in range(m, 0, -1):
            if weights[i - 1] <= j:
                f[j] = max(f[j], f[j - weights[i - 1]] + values[i - 1])

    return f[m]


def solve_saving_space(num):
    """
    解决背包问题
    :param num: 选择要解决的问题, 1表示第一组, 2表示第二组
    :return: None
    """
    n, w, v = read_file(num)
    max_value = zero_one_knapsack_saving_space(n, w, v)
    print(f"第{num}组, 最大价值为: ", max_value)


if __name__ == "__main__":
    solve(1)
    solve(2)

    solve_saving_space(1)
    solve_saving_space(2)
