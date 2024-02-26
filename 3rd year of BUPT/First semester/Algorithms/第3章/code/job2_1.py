import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


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
    """
    基站类
    """

    def __init__(self, eNodeB_id, latitude, longitude, number):
        """
        基站类的初始化
        :param eNodeB_id:  基站编号
        :param latitude:   x坐标
        :param longitude:  y坐标
        :param number:   逆时针顺序标号
        """
        self.eNodeB_id = eNodeB_id
        self.x = longitude
        self.y = latitude
        self.number = number


def initialize_stations(path):
    """
    初始化基站
    :param path: 基站数据文件路径
    :return: 基站列表
    """
    data = pd.read_excel(path, sheet_name=1)
    # 删除未命名的列和行
    data = data.loc[:, ~data.columns.str.contains("^Unnamed")]
    data = data.dropna(axis=0, how="any")

    eNodeB_id = data["ENODEBID"].tolist()
    longitude = data["LONGITUDE"].tolist()
    latitude = data["LATITUDE"].tolist()
    number = data["逆时针顺序标号"].tolist()
    stations = [
        Station(eNodeB_id, lon, lat, num)
        for eNodeB_id, lon, lat, num in zip(eNodeB_id, longitude, latitude, number)
    ]
    return stations


def draw_stations(stations, s, title="Optimal Triangulation"):
    """
    绘制基站图
    :param stations: 基站列表
    :param s: s[i][j]表示从i到j的最优三角剖分的最小权值对应的k
    """
    n = len(stations)
    # 画图展示最优三角剖分
    plt.figure(figsize=(10, 10))
    plt.xlim(
        min([station.x for station in stations]) - 0.01,
        max([station.x for station in stations]) + 0.01,
    )
    plt.ylim(
        min([station.y for station in stations]) - 0.01,
        max([station.y for station in stations]) + 0.01,
    )
    plt.title(title)
    plt.xlabel("Longitude")
    plt.ylabel("Latitude")
    plt.grid()
    plt.axis("equal")
    # 画出基站的位置
    for station in stations:
        plt.scatter(station.x, station.y, c="r", s=30)
        plt.text(station.x, station.y, int(station.number), fontsize=10)

    # 递归打印最优三角剖分
    def print_optimal_triangulation(l, r):
        if l + 1 >= r:  # 递归边界
            return
        print(l, s[l][r], r)
        plt.plot(  # 连接l和k=s[l][r]
            [stations[l].x, stations[s[l][r]].x],
            [stations[l].y, stations[s[l][r]].y],
            c="b",
        )
        plt.plot(  # 连接k=s[l][r]和r
            [stations[s[l][r]].x, stations[r].x],
            [stations[s[l][r]].y, stations[r].y],
            c="b",
        )
        print_optimal_triangulation(l, s[l][r])
        print_optimal_triangulation(s[l][r], r)

    print_optimal_triangulation(0, n - 1)
    plt.show()


# 凸多边形最优三角剖分问题
def optimal_triangulation(stations):
    """
    凸多边形最优三角剖分问题
    :param stations: 基站列表
    :return: 最优三角剖分的最小权值
    """
    n = len(stations)
    # dp[i][j]表示从i到j的最优三角剖分的最小权值
    dp = [[0 for _ in range(n)] for _ in range(n)]
    # s[i][j]表示从i到j的最优三角剖分的最小权值对应的k
    s = [[0 for _ in range(n)] for _ in range(n)]
    # 初始化dp[i][i+1]
    for i in range(n - 1):
        dp[i][i + 1] = 0
    # 从小到大枚举区间长度
    for len_ in range(3, n + 1):
        # 从小到大枚举左端点
        for l in range(n - len_ + 1):  # l + len_ - 1 < n
            r = l + len_ - 1
            # 从l+1到r-1枚举k
            for k in range(l + 1, r):
                # 计算三角形lkr的权值
                weight = (
                    calculate_distance(
                        stations[l].y, stations[l].x, stations[k].y, stations[k].x
                    )
                    + calculate_distance(
                        stations[k].y, stations[k].x, stations[r].y, stations[r].x
                    )
                    + calculate_distance(
                        stations[r].y, stations[r].x, stations[l].y, stations[l].x
                    )
                )
                # 更新dp[l][r]
                if dp[l][r] == 0 or dp[l][r] > dp[l][k] + dp[k][r] + weight:
                    dp[l][r] = dp[l][k] + dp[k][r] + weight
                    s[l][r] = k

    # 画图展示最优三角剖分
    draw_stations(stations, s)

    return dp[0][n - 1]


# 凸多边形最优三角剖分问题,O(n^2)的近似算法
def optimal_triangulation_approx(stations, k):
    """
    凸多边形最优三角剖分问题,O(n^2)的近似算法
    :param stations: 基站列表
    :param k: 随机选取k个点,最大为5
    :return: 最优三角剖分的最小权值
    """
    n = len(stations)
    # dp[i][j]表示从i到j的最优三角剖分的最小权值
    dp = [[0 for _ in range(n)] for _ in range(n)]
    # s[i][j]表示从i到j的最优三角剖分的最小权值对应的k
    s = [[0 for _ in range(n)] for _ in range(n)]
    # 初始化dp[i][i+1]
    for i in range(n - 1):
        dp[i][i + 1] = 0
    # 从小到大枚举区间长度
    for len_ in range(3, n + 1):
        # 从小到大枚举左端点
        for l in range(n - len_ + 1):  # l + len_ - 1 < n
            r = l + len_ - 1
            # 贪心策略，直接从随机选择的k个分割点中选取最好的分割点，范围[l+1,r-1]
            # 随机选取k个分割点
            if r - l - 1 > k:
                split_points = np.random.choice(range(l + 1, r), k, replace=False)
            else:  # 如果区间长度小于等于k，则选取所有的分割点
                split_points = range(l + 1, r)
            # 选取最好的分割点
            for k in split_points:
                # 计算三角形lkr的权值
                weight = (
                    calculate_distance(
                        stations[l].y, stations[l].x, stations[k].y, stations[k].x
                    )
                    + calculate_distance(
                        stations[k].y, stations[k].x, stations[r].y, stations[r].x
                    )
                    + calculate_distance(
                        stations[r].y, stations[r].x, stations[l].y, stations[l].x
                    )
                )
                # 更新dp[l][r]
                if dp[l][r] == 0 or dp[l][r] > dp[l][k] + dp[k][r] + weight:
                    dp[l][r] = dp[l][k] + dp[k][r] + weight
                    s[l][r] = k

    draw_stations(stations, s, title="Optimal Triangulation Approx")

    return dp[0][n - 1]


if __name__ == "__main__":
    print('21个基站凸多边形数据')
    stations = initialize_stations(
        r"附件3-1.21个基站凸多边形数据-2023.xls"
    )
    print('最优三角剖分的最小权值{}'.format(optimal_triangulation(stations)))
    print('最优三角剖分的近似最小权值{}'.format(optimal_triangulation_approx(stations, 5)))

    print('\n29个基站凸多边形数据')
    stations = initialize_stations(
        r"附件3-2.29个基站凸多边形数据-2023.xls"
    )
    print('最优三角剖分的最小权值{}'.format(optimal_triangulation(stations)))
    print('最优三角剖分的近似最小权值{}'.format(optimal_triangulation_approx(stations, 5)))
