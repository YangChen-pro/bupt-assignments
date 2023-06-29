#include <bits/stdc++.h>
#include "navigation.h"
#include "QDebug"
#include <synchapi.h>

using namespace std;
static const int N = 127;          // 节点数最大值
static const int INF = 0x3f3f3f3f; // 距离最大值
typedef pair<int, int> PII;        // 存储坐标
#define x first
#define y second

// 定义一个结构体来存储最短路径相关信息
struct path
{
    int cnt = 0;     // 路径上的节点总数
    int s[N];        // 路径上的节点编号
    double dist = 0; // 路径长度
    int mode;
    double congest; // 拥塞程度
};

// 定义一个结构体来表示邻接表中的一个节点
struct Node
{
    int dest;
    Node* next;
    // flag用于标记是否是只可步行的边 =1只可步行 =0即可步行又可以骑车
    int flag;
};

// 定义一个结构体来表示邻接表
struct AdjList
{
    string place_name;
    PII loc;
    Node* head;
};

// 定义一个类来表示图
class Graph
{
public:
    // 构造函数
    Graph();

    // 析构函数
    ~Graph();

    // 初始化顶点表
    void initVNode();

    // BKDRHash
    // 用于将地点名称的字符串映射到一个非负整数作为数组的下标值
    unsigned int BKDRHash(char* str);

    // 函数用来向图中添加一条边
    void addEdge(string src, string dest, int flag);

    // 计算地名对应的数组下标值
    unsigned int getIndex(string name);

    // 函数用来打印图的邻接表表示
    void printGraph();

    // dijkstra算法
    // 时间复杂度O(mlogn)，其中m为边数，n为点数
    void dijkstra(string src, string dest, int mode);

    // 找到最短路径
    void getPath(unsigned int s, int* prev, int* transportation, int* temp_dist, int mode, double* congestion);

    // 输出最短路径
    void printPath(unsigned int s, unsigned int d, int mode);

    // 禁忌搜索算法
    // 禁忌（Tabu Search）算法是一种亚启发式(meta-heuristic)随机搜索算法
    // 它从一个初始可行解出发，选择一系列的特定搜索方向（移动）作为试探，选择实现让特定的目标函数值变化最多的移动
    // 为了避免陷入局部最优解，TS搜索中采用了一种灵活的“记忆”技术
    // 对已经进行的优化过程进行记录和选择，指导下一步的搜索方向，这就是Tabu表的建立
    // 计算路径长度
    int calcPathLen(const vector<int>& path, int mode);
    // 交换两个位置的值
    void swapVal(int& a, int& b);
    // 生成候选解
    vector<int> generateCandidate(const vector<int>& cur_path);
    // 判断路径是否在禁忌表中
    bool isInTabuList(const vector<int>& path);
    // 更新禁忌表和禁忌期
    void updateTabuList(const vector<int>& cur_path, const vector<int>& new_path);
    // 禁忌搜索算法
    vector<int> tabuSearch(string src, vector<string> dst, int mode);

    // 利用dfs算法求解TSP问题
    vector<int> dfsTsp(string src, vector<string> dst, int mode);
    void dfs(vector<int>& nums, vector<int>& visited, vector<int>& permutation, vector<int>& best_path, int& lowest_cost, int mode);

    // 显示导航
    void show_dijk(string src, string dest, int mode);
    void show_dfs(string src, vector<string> dst, int mode);
    void show_tabu(string src, vector<string> dst, int mode);

public:
    AdjList* array;
    Navigation* n;
};
