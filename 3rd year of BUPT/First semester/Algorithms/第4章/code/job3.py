import pandas as pd
import matplotlib.pyplot as plt
import networkx as nx
import heapq
import matplotlib

matplotlib.rc("font", family="SimHei", weight="bold")  # 解决中文乱码
plt.rcParams["axes.unicode_minus"] = False  # 解决负号无法显示的问题


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


def draw(adjacency_matrix, edges, title):
    """
    绘制最小生成树的图形
    :param adjacency_matrix: 邻接矩阵
    :param edges: 最小生成树的边
    :param title: 图形标题
    """
    G = nx.Graph()
    n = len(adjacency_matrix)

    # 添加顶点
    for i in range(n):
        G.add_node(i)

    min_edge = []

    if edges[0][0] is None:  # 去掉第一个空边(None, 0)
        min_edge = edges[1:]
    else:  # Kruskal算法没有空边
        min_edge = edges

    # 添加边
    for i in range(n):
        for j in range(n):
            if adjacency_matrix[i][j] != -1:
                G.add_edge(i, j, weight=adjacency_matrix[i][j])

    # 绘制图形
    pos = nx.spring_layout(G, seed=1)
    fig, ax = plt.subplots()
    ax.set_title(title)
    nx.draw(G, pos, node_size=100, node_color='red', edge_color='black', width=3.0, alpha=0.6, ax=ax)
    nx.draw_networkx_labels(G, pos, font_size=13, font_color='black', alpha=1.0)

    # 绘制最小生成树的边
    nx.draw_networkx_edges(G, pos, edgelist=min_edge, width=3.0, alpha=0.6, edge_color='red')
    plt.savefig(title + '.png')
    plt.show()

# 最小生成树,Prim算法,O(n^2)
def Prim(adjacency_matrix):
    """
    最小生成树,Prim算法,O(n^2)
    :param adjacency_matrix: 邻接矩阵
    :return: 最小生成树的权值
    """
    n = len(adjacency_matrix)
    min_cost = 0
    # 初始化
    lowcost = [float('inf')] * n
    lowcost[0] = 0
    visited = [False] * n
    edges = []  # 最小生成树的边

    for _ in range(n):
        min_dis = float('inf')
        u = -1
        for i in range(n):
            if not visited[i] and lowcost[i] < min_dis:
                min_dis = lowcost[i]
                u = i
        if u == -1:
            break
        pre_u = next((pre_u for pre_u in range(n) if adjacency_matrix[pre_u][u] == min_dis), None)
        edges.append((pre_u, u))
        min_cost += min_dis
        visited[u] = True
        for v in range(n):
            if adjacency_matrix[u][v] == -1:
                continue
            if not visited[v] and adjacency_matrix[u][v] < lowcost[v]:
                lowcost[v] = adjacency_matrix[u][v]

    if len(visited) != n:
        print('不连通')
        return None

    return min_cost, edges


# 最小生成树,Prim算法,O((V+E)logV)
def Prim_saving_time(adjacency_matrix):
    """
    最小生成树,Prim算法,O((V+E)logV)
    :param adjacency_matrix: 邻接矩阵
    :return: 最小生成树的权值
    """
    n = len(adjacency_matrix)
    min_cost = 0
    # 初始化
    lowcost = [float('inf')] * n
    lowcost[0] = 0
    visited = [False] * n
    heap = [(0.0, 0)]  # (权值, 下标)
    edges = []  # 最小生成树的边

    while heap:
        cost, u = heapq.heappop(heap)
        if visited[u]:
            continue
        min_cost += cost
        visited[u] = True
        pre_u = next((pre_u for pre_u in range(n) if adjacency_matrix[pre_u][u] == cost), None)
        edges.append((pre_u, u))
        for v in range(n):
            if adjacency_matrix[u][v] == -1:
                continue
            if not visited[v] and adjacency_matrix[u][v] < lowcost[v]:
                lowcost[v] = adjacency_matrix[u][v]
                heapq.heappush(heap, (lowcost[v], v))

    if len(visited) != n:
        print('不连通')
        return None

    return min_cost, edges


# 最小生成树,Kruskal算法,O(ElogV)
def Kruskal(adjacency_matrix):
    """
    最小生成树,Kruskal算法,O(ElogE)
    :param adjacency_matrix: 邻接矩阵
    :return: 最小生成树的权值
    """

    def find(parent, i):
        """
        查找i的根节点
        :param parent: parent[i]表示i的父节点
        :param i: 节点i
        :return: i的根节点
        """
        while parent[i] != i:
            i = parent[i]
        return i

    def union(parent, i, j):
        """
        合并i和j所在的集合
        :param parent: parent[i]表示i的父节点
        :param i: 节点i
        :param j: 节点j
        """
        i_root = find(parent, i)
        j_root = find(parent, j)
        if i_root != j_root:
            parent[i_root] = j_root

    n = len(adjacency_matrix)
    edges_list = []
    # 初始化
    for i in range(n):
        for j in range(n):
            if adjacency_matrix[i][j] != -1:
                edges_list.append((i, j, adjacency_matrix[i][j]))
    edges_list.sort(key=lambda x: x[2])
    edges = []  # 最小生成树的边
    min_cost = 0
    parent = [i for i in range(n)]

    for u, v, cost in edges_list:
        if find(parent, u) != find(parent, v):
            union(parent, u, v)
            min_cost += cost
            edges.append((u, v))

        if len(edges) == n - 1:
            break

    if len(edges) != n - 1:
        print('不连通')
        return None

    return min_cost, edges


def solve(num):
    adjacency_matrix, station_ids = init_data(num)
    min_cost, edges = Prim(adjacency_matrix)
    print('Prim算法最小生成树的权值: {:.2f}'.format(min_cost))
    draw(adjacency_matrix, edges, '数据集' + num + '的Prim算法')
    min_cost, edges = Kruskal(adjacency_matrix)
    print('Kruskal算法最小生成树的权值: {:.2f}'.format(min_cost))
    draw(adjacency_matrix, edges, '数据集' + num + '的Kruskal算法')
    min_cost_saving_time, edges = Prim_saving_time(adjacency_matrix)
    if min_cost - min_cost_saving_time < 1e-9:
        print('经检验, Prim算法和Prim_saving_time算法结果相同')
    print()


if __name__ == '__main__':
    solve('1')
    solve('2')
