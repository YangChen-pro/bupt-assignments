import numpy as np
import pandas as pd


def degree_to_radian(degree):
    """
    将给定的经纬度转化为弧度
    :param degree: 1个输入(经纬度)
    :return: 转化为的弧度
    """
    return degree * np.pi / 180


def calculate_distance(lat1, lon1, lat2, lon2):
    """
    距离公式
    :param lat1: 纬度1
    :param lon1: 经度1
    :param lat2: 纬度2
    :param lon2: 经度2
    :return: 距离/m,保留6位小数
    """
    if abs(lat1 - lat2) < 1e-6 and abs(lon1 - lon2) < 1e-6:
        return 0
    rad_lat1 = degree_to_radian(lat1)
    rad_lon1 = degree_to_radian(lon1)
    rad_lat2 = degree_to_radian(lat2)
    rad_lon2 = degree_to_radian(lon2)
    # R为赤道半径/m
    R = 6378.137 * 1000
    dis = R * np.arccos(
        np.cos(rad_lat1) * np.cos(rad_lat2) * np.cos(rad_lon1 - rad_lon2)
        + np.sin(rad_lat1) * np.sin(rad_lat2)
    )
    return round(dis, 6)


class Station:
    def __init__(self, eNodeB_id, latitude, longitude):
        self.eNodeB_id = eNodeB_id
        self.x = longitude
        self.y = latitude


def merge(stations, l, mid, r):
    """
    合并两个有序数组，区间[l,mid)和[mid,r)，但是按照y坐标排序
    :param stations: 基站列表
    :param l: 左边界
    :param mid: 中间位置
    :param r: 右边界
    """
    merged = sorted(stations[l:r], key=lambda station: station.y)
    stations[l:r] = merged


def nearest_pair(stations, l, r, min_dis, lesser_dis):
    """
    求解最近点对，区间为[l,r)
    :param stations: 基站列表
    :param l: 左边界
    :param r: 右边界
    :param min_dis: 最近距离 (距离，eNodeB_id1，eNodeB_id2)
    :param lesser_dis: 次近距离 (距离，eNodeB_id1，eNodeB_id2)
    :return: 最近距离 (距离，eNodeB_id1，eNodeB_id2)，次近距离 (距离，eNodeB_id1，eNodeB_id2)
    """
    if r - l <= 1:
        return min_dis, lesser_dis
    mid = l + (r - l) // 2
    # 递归求解左右两边的最近点对
    min_dis, lesser_dis = nearest_pair(stations, l, mid, min_dis, lesser_dis)
    min_dis, lesser_dis = nearest_pair(stations, mid, r, min_dis, lesser_dis)
    # 合并两个有序数组，但是按照y坐标排序
    merge(stations, l, mid, r)
    # 选取中间区域的点
    mid_line = stations[mid].x  # 中线
    selected = []
    for i in range(l, r):  # 选取中线左右两边距离中线小于min_dis的点
        if abs(stations[i].x - mid_line) < min_dis[0]:
            selected.append(stations[i])
    # 计算最近点对
    for i in range(len(selected)):
        for j in range(i + 1, len(selected)):
            if selected[j].y - selected[i].y >= min_dis[0]:  # 剪枝，y距离大于min_dis的点不用计算
                break
            dis = calculate_distance(
                selected[i].y, selected[i].x, selected[j].y, selected[j].x
            )
            if dis == 0:  # 剪枝，距离为0的点对不用计算
                continue
            if dis < min_dis[0]:  # 更新最近点对
                lesser_dis = min_dis
                min_dis = (dis, selected[i].eNodeB_id, selected[j].eNodeB_id)
            elif min_dis[0] < dis < lesser_dis[0]:  # 更新次近点对
                lesser_dis = (dis, selected[i].eNodeB_id, selected[j].eNodeB_id)

    return min_dis, lesser_dis


if __name__ == "__main__":
    # 读取基站数据文件
    data = pd.read_excel(
        r"02-1 1033个基站数据.xls"
    )
    # 删除未命名的列
    data = data.loc[:, ~data.columns.str.contains("^Unnamed")]

    eNodeB_id = data["ENODEBID"].tolist()
    longitude = data["LONGITUDE"].tolist()
    latitude = data["LATITUDE"].tolist()

    # 创建对象的列表
    stations = [
        Station(eNodeB_id, lon, lat)
        for eNodeB_id, lon, lat in zip(eNodeB_id, longitude, latitude)
    ]
    stations_copy = stations.copy()  # 复制一份用于验证结果的正确性

    min_dis = (1e10, -1, -1)
    lesser_dis = (1e10, -1, -1)

    stations.sort(key=lambda station: station.x)

    # 求解最近点对
    min_dis, lesser_dis = nearest_pair(stations, 0, len(stations), min_dis, lesser_dis)
    print("最近点对距离为：", min_dis[0], "，eNodeB_id分别为：", min_dis[1], min_dis[2])
    print("次近点对距离为：", lesser_dis[0], "，eNodeB_id分别为：", lesser_dis[1], lesser_dis[2])

    # 验证结果的正确性
    min_dis_ = 1e10
    lesser_dis_ = 1e10
    min_id1, min_id2, lesser_id1, lesser_id2 = -1, -1, -1, -1
    for i in range(len(stations_copy)):
        for j in range(i + 1, len(stations_copy)):
            dis = calculate_distance(
                stations_copy[i].y,
                stations_copy[i].x,
                stations_copy[j].y,
                stations_copy[j].x,
            )
            if dis == 0:  # 剪枝，距离为0的点对不用计算
                continue
            if dis < min_dis_:
                lesser_dis_ = min_dis_
                lesser_id1, lesser_id2 = min_id1, min_id2
                min_dis_ = dis
                min_id1, min_id2 = (
                    stations_copy[i].eNodeB_id,
                    stations_copy[j].eNodeB_id,
                )
            elif min_dis_ < dis < lesser_dis_:
                lesser_dis_ = dis
                lesser_id1, lesser_id2 = (
                    stations_copy[i].eNodeB_id,
                    stations_copy[j].eNodeB_id,
                )
    print("\n通过遍历求解的结果：")
    print("最近点对距离为：", min_dis_, "，eNodeB_id分别为：", min_id1, min_id2)
    print("次近点对距离为：", lesser_dis_, "，eNodeB_id分别为：", lesser_id1, lesser_id2)
