import pandas as pd


def bubble_sort(arr, low, high):
    """
    冒泡排序
    :param arr: 待排序列表
    :param low: 左边界
    :param high: 右边界
    :return: 从低到高排序的列表
    """
    n = high - low + 1
    for i in range(n):
        is_swapped = False
        for j in range(n - i - 1):
            if arr[j + low] > arr[j + 1 + low]:
                arr[j + low], arr[j + 1 + low] = arr[j + 1 + low], arr[j + low]
                is_swapped = True
        if not is_swapped:
            break


# 全局变量，记录选择划分过程的递归层次
recursion_level = 0


def clear_level():
    """
    将递归层次清零
    :return:
    """
    global recursion_level
    recursion_level = 0


def linear_select(arr, low, high, k, current_level=1):
    """
    线性时间选择算法
    :param arr:待划分数组
    :param low: 左边界
    :param high: 右边界
    :param k: 选择的第k小的元素
    :return:
    """
    global recursion_level
    recursion_level = max(recursion_level, current_level)
    if high - low + 1 < 20:
        # 如果数组长度小于20，则直接排序
        bubble_sort(arr, low, high)
        return arr[low + k - 1]

    # 5个元素一组，分别排序
    for i in range(low, high - 4, 5):
        bubble_sort(arr, i, i + 4)
        # 将中位数放到数组最前面
        arr[low + (i - low) // 5], arr[i + 2] = arr[i + 2], arr[low + (i - low) // 5]
    # 得到中位数的中位数
    cnt = (high - low + 1) // 5
    pivot = linear_select(arr, low, low + cnt - 1, cnt // 2 + 1, current_level + 1)
    # 得到pivot的下标
    pivot = arr.index(pivot)
    # 将pivot放到数组最前面
    arr[low], arr[pivot] = arr[pivot], arr[low]
    # 一分为三
    pivot = partition_three(arr, low, high)
    # 递归寻找
    if pivot - low + 1 == k:
        return arr[pivot]
    elif pivot - low + 1 > k:
        return linear_select(arr, low, pivot - 1, k, current_level + 1)
    else:
        return linear_select(
            arr, pivot + 1, high, k - (pivot - low + 1), current_level + 1
        )


def partition_three(arr, low, high):
    """
    一分为三,基准元素为数组第一个元素
    :param arr:待划分数组
    :param low: 左边界
    :param high: 右边界
    :return: 划分后基准元素的下标
    """
    pivot = arr[low]
    # 将数组分为三部分
    i = low + 1
    j = high
    while True:
        while i <= j and arr[i] <= pivot:
            i += 1
        while i <= j and arr[j] >= pivot:
            j -= 1
        if i > j:
            break
        arr[i], arr[j] = arr[j], arr[i]
    arr[low], arr[j] = arr[j], arr[low]
    return j


if __name__ == "__main__":
    # 读取基站数据文件
    data = pd.read_excel(
        r"02-1 1033个基站数据.xls"
    )
    # 删除未命名的列
    data = data.loc[:, ~data.columns.str.contains("^Unnamed")]

    # 提取k-dist列的数据
    k_dist_values = data["K_DIST"].tolist()
    k_dist_copy = k_dist_values.copy()  # 复制一份用于验证结果的正确性

    # 选择最小的元素
    min_dist_values = linear_select(k_dist_values, 0, len(k_dist_values) - 1, 1)
    print("最小的元素为：", min_dist_values)
    print("递归最大层次为：", recursion_level, "\n")

    # 选择第5小的元素
    clear_level()
    min_dist_values = linear_select(k_dist_values, 0, len(k_dist_values) - 1, 5)
    print("第5小的元素为：", min_dist_values)
    print("递归最大层次为：", recursion_level, "\n")

    # 选择第50小的元素
    clear_level()
    min_dist_values = linear_select(k_dist_values, 0, len(k_dist_values) - 1, 50)
    print("第50小的元素为：", min_dist_values)
    print("递归最大层次为：", recursion_level, "\n")

    # 选择最大的元素
    clear_level()
    min_dist_values = linear_select(
        k_dist_values, 0, len(k_dist_values) - 1, len(k_dist_values)
    )
    print("最大的元素为：", min_dist_values)
    print("递归最大层次为：", recursion_level, "\n")

    # 验证结果的正确性
    k_dist_copy.sort()
    print("通过排序验证结果的正确性：")
    print("最小的元素为：", k_dist_copy[0])
    print("第5小的元素为：", k_dist_copy[4])
    print("第50小的元素为：", k_dist_copy[49])
    print("最大的元素为：", k_dist_copy[-1])
