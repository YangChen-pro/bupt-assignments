import pandas as pd
import heapq
import numpy as np


def init_data(num):
    """
    初始化数据
    :param num: 数据集编号,[1,2]
    :return: 邻接矩阵, 下标对应的基站id
    """
    df = pd.read_excel('附件1-1.基站图的邻接矩阵-v1-23.xls', sheet_name='数据' + num, header=None)
    size = 0
    if num == '1':
        size = 24
    elif num == '2':
        size = 44
    # 邻接矩阵
    adjacency_matrix = df.values[2:size, 2:size].astype(float)
    # 下标对应的基站id
    station_ids = df.values[2:size, 1].astype(int)
    return adjacency_matrix, station_ids


# 单源最短路径dijkstra算法, 时间复杂度O(V^2)
def dijkstra(adjacency_matrix, start):
    """
    单源最短路径dijkstra算法, 时间复杂度O(V^2)
    :param adjacency_matrix: 邻接矩阵
    :param start: 起点
    :return: dis[i]表示从start到i的最短距离
    """
    n = len(adjacency_matrix)
    # 初始化
    dis = [float('inf')] * n
    dis[start] = 0
    visited = [False] * n

    for i in range(n):
        u = -1
        min_dis = float('inf')
        for j in range(n):
            if not visited[j] and dis[j] < min_dis:
                u = j
                min_dis = dis[j]
        if u == -1:
            break
        visited[u] = True
        for v in range(n):
            if adjacency_matrix[u][v] == -1:
                continue
            if not visited[v] and dis[v] > dis[u] + adjacency_matrix[u][v]:
                dis[v] = dis[u] + adjacency_matrix[u][v]

    if len(visited) != n:
        print('不连通')
        return None

    return dis


# 单源最短路径dijkstra算法, 时间复杂度O((V+E)logV)
def dijkstra_saving_time(adjacency_matrix, start):
    """
    单源最短路径dijkstra算法, 时间复杂度O((V+E)logV)
    :param adjacency_matrix: 邻接矩阵
    :param start: 起点
    :return: dis[i]表示从start到i的最短距离
    """
    n = len(adjacency_matrix)
    # 初始化
    dis = [float('inf')] * n
    dis[start] = 0
    visited = [False] * n

    heap = [(0.0, start)]

    while heap:
        d, u = heapq.heappop(heap)
        if visited[u]:
            continue
        visited[u] = True
        for v in range(n):
            if adjacency_matrix[u][v] == -1:
                continue
            if not visited[v] and dis[v] > dis[u] + adjacency_matrix[u][v]:
                dis[v] = dis[u] + adjacency_matrix[u][v]
                heapq.heappush(heap, (dis[v], v))

    if len(visited) != n:
        print('不连通')
        return None

    return dis


def solve(num, start, end):
    """
    解决问题
    :param num: 数据集编号, [1,2]
    :param start: 起点id
    :param end: 终点id
    """
    adjacency_matrix, station_ids = init_data(num)
    start = np.where(station_ids == start)[0][0]
    end = np.where(station_ids == end)[0][0]
    dist = dijkstra(adjacency_matrix, start)
    print(f'\033[93m以{station_ids[start]}为起点，到各点的最短距离为\033[0m')
    for i in range(len(dist)):
        print(f'{station_ids[i]}: {dist[i]:.2f}')
    print(f'\033[92m以{station_ids[start]}为起点，到{station_ids[end]}的最短距离为:{dist[end]:.2f}\033[0m')
    # 验证优化后的dijkstra算法
    new_dist = dijkstra_saving_time(adjacency_matrix, start)
    if dist == new_dist:
        print('\033[92m经验证, 优化后的dijkstra算法正确\033[0m')
    else:
        print('\033[91m经验证, 优化后的dijkstra算法错误\033[0m')
    print()


if __name__ == '__main__':
    solve('1', 567443, 33109)
    solve('2', 565845, 565667)
