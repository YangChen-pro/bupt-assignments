#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <chrono>

#define NO_EDGE 99999

/*
 * fileName: 输入文件名
 * n: 节点数
 * graph: 邻接矩阵
 * id2index: id到下标的映射
 */
void input(const char *fileName, int &n, std::vector<std::vector<double>> &graph, std::unordered_map<int, int> &id2index, std::vector<std::vector<double>> &min2)
{
    std::ifstream inputFile(fileName);
    if (!inputFile)
    {
        std::cout << "File not found!" << std::endl;
        return;
    }

    std::string line;
    std::getline(inputFile, line); // 读取第一行（编号行）
    for (int i = 0; i < line.size(); ++i)
        if (line[i] == '\t')
            ++n;
    n--;
    min2.resize(n, std::vector<double>(2, NO_EDGE)); // 初始化min2，n行2列，每个元素初始化为NO_EDGE
    std::stringstream lineStream(line);

    for (int i = 0; i < n; ++i)
    {
        std::string temp;
        lineStream >> temp;
        id2index[i] = std::stoi(temp);
    }

    // 读取id行并忽略
    std::getline(inputFile, line);

    // 读取边的权值矩阵
    graph.resize(n, std::vector<double>(n));
    for (int i = 0; i < n; ++i)
    {
        std::getline(inputFile, line);
        std::stringstream lineStream(line);
        // 忽略前2列（编号和id）
        std::string temp;
        lineStream >> temp;
        lineStream >> temp;
        for (int j = 0; j < n; ++j)
        {
            lineStream >> graph[i][j];
            if (graph[i][j] < min2[i][0]) // 比最小的小，更新最小的和次小的
            {
                min2[i][1] = min2[i][0];
                min2[i][0] = graph[i][j];
            }
            else if (graph[i][j] < min2[i][1]) // 比次小的小，更新次小的
            {
                min2[i][1] = graph[i][j];
            }
        }
    }

    inputFile.close();
}

/*
 * 求与当前节点最近的未访问过的节点的下标
 * graph: 邻接矩阵
 * vis: 标记节点是否被访问过
 * current_city: 当前所在的城市
 * exclude: 排除的节点
 * 返回值：下一个城市的下标
 */
int findMin(std::vector<std::vector<double>> &graph, std::vector<bool> &vis, int current_city, std::vector<bool> &exclude)
{
    double min_edge = NO_EDGE; // 最小边，初始化
    int next_city = -1;
    for (int i = 0; i < graph.size(); ++i)
    {
        // 如果当前节点到i有边小于最小值，且i没有被访问过，且i不在排除的节点中
        if (graph[current_city][i] < min_edge && !vis[i] && !exclude[i])
        {
            min_edge = graph[current_city][i]; // 维护最小边
            next_city = i;
        }
    }
    return next_city;
}

/*
 * 计算当前路径的上界
 * graph: 邻接矩阵
 * vis: 标记节点是否被访问过
 * current_city: 当前所在的城市
 * start_city: 起始城市
 * count: 已经访问过的节点数
 * 返回值：当前路径的上界
 */
double calculateUpperBound(std::vector<std::vector<double>> &graph, std::vector<bool> &vis, int current_city, int start_city, int count)
{
    if (count == graph.size())
    {
        if (graph[current_city][start_city] != NO_EDGE)
        {
            return graph[current_city][start_city];
        }
        else
            return -1;
    }
    std::vector<bool> exclude(graph.size(), false); // 排除的节点
    int next_city = findMin(graph, vis, current_city, exclude);
    while (next_city != -1)
    {
        vis[next_city] = true;
        double temp = calculateUpperBound(graph, vis, next_city, start_city, count + 1);
        if (temp != -1) // 找到了一条路径
            return temp + graph[current_city][next_city];
        vis[next_city] = false; // 回溯
        exclude[next_city] = true;
        next_city = findMin(graph, vis, current_city, exclude);
    }
    return -1; // 没有找到路径，返回-1，回溯
}

/*
 * 计算当前路径的下界
 * graph: 邻接矩阵
 * min2: 存储每个节点最短的2条路径的长度
 * path: 当前路径
 * 返回值：当前路径的下界
 */
double calculateLowerBound(std::vector<std::vector<double>> &graph, std::vector<std::vector<double>> &min2, std::vector<int> &path, std::vector<bool> &vis)
{
    int u = path[0], v = path[path.size() - 1]; // u，v记录当前路径的起点和终点
    double lowerBound = 0;
    int n = graph.size();
    for (int i = 0; i < path.size() - 1; i++) // 计算当前路径的花费
        lowerBound += graph[path[i]][path[i + 1]];
    lowerBound *= 2;
    if (path.size() >= 2) // 路径中有2个点以上
    {
        double min_edge1 = NO_EDGE, min_edge2 = NO_EDGE; // 分别记录回到u的最小边和从v出发的最小边
        for (int i = 0; i < n; i++)
        {
            if (graph[i][u] < min_edge1)
                min_edge1 = graph[i][u];
            if (graph[v][i] < min_edge2)
                min_edge2 = graph[v][i];
        }
        lowerBound += min_edge1 + min_edge2;
    }
    else // 只有1个点，直接加上最小的2条边
    {
        lowerBound += min2[u][0] + min2[u][1];
    }
    for (int i = 0; i < n; i++)
    {
        if (!vis[i]) // 如果i不在当前路径中
        {
            lowerBound += min2[i][0] + min2[i][1]; // 加上i的最小的2条边
        }
    }
    return lowerBound / 2;
}

struct State
{
    int current_city;      // 当前所在的城市
    int count;             // 已经访问过的节点数
    double now_cost;       // 当前路径的花费
    double lowerBound;     // 当前路径的下界
    std::vector<int> path; // 当前路径
    std::vector<bool> vis; // 标记节点是否被访问过
    // 构造函数
    State(int current_city, int count, double now_cost, double lowerBound, std::vector<int> &path, std::vector<bool> &vis)
        : current_city(current_city), count(count), now_cost(now_cost), lowerBound(lowerBound), path(path), vis(vis) {}

    // 重载大于号，按照lowerBound从小到大排序
    friend bool operator>(const State &s1, const State &s2)
    {
        return s1.lowerBound > s2.lowerBound;
    }
};

/*
 * start_city: 起始节点
 * min_cost: 最小花费
 * best_path: 最优路径
 * graph: 邻接矩阵
 * min2: 存储每个节点最短的2条路径的长度
 * sum_node: 搜索节点数
 * upperBound: 上界
 */
void TSP(int start_city, double &min_cost, std::vector<int> &best_path, std::vector<std::vector<double>> &graph, std::vector<std::vector<double>> &min2, long long &sum_node, double upperBound)
{
    std::priority_queue<State, std::vector<State>, std::greater<State>> pq; // 优先队列，按照lowerBound从小到大排序
    std::vector<bool> vis(graph.size(), false);                             // 标记节点是否被访问过
    vis[start_city] = true;                                                 // 起始节点标记为已访问
    std::vector<int> path;                                                  // 当前路径
    path.push_back(start_city);                                             // 起始节点加入路径
    double lowerBound = calculateLowerBound(graph, min2, path, vis);        // 计算当前路径的下界
    // std::cout << "lowerBound: " << lowerBound << std::endl;                 // 输出下界
    pq.push(State(start_city, 1, 0, lowerBound, path, vis)); // 起始节点加入优先队列
    while (!pq.empty())
    {
        State state = pq.top(); // 取出优先队列中的第一个状态
        pq.pop();
        sum_node++;                        // 搜索节点数加1
        if (state.lowerBound > upperBound) // 如果当前状态的下界大于上界，剪枝
            continue;
        if (state.count == graph.size()) // 所有节点都访问过了，回到起始节点
        {
            // 如果当前节点到起始节点有边，且当前花费加上当前节点到起始节点的花费小于最小花费
            if (graph[state.current_city][start_city] != NO_EDGE && state.now_cost + graph[state.current_city][start_city] < min_cost)
            {
                min_cost = state.now_cost + graph[state.current_city][start_city];
                best_path = state.path;
                best_path.push_back(start_city);
                if (min_cost <= state.lowerBound) // 如果最小花费小于等于下界，直接返回，不再继续搜索，因为已经找到了最优解
                    return;
                if (min_cost < upperBound) // 如果最小花费小于上界，更新上界，并删除优先队列中大于当前最小花费的状态
                {
                    upperBound = min_cost;
                    std::vector<State> temp;
                    while (!pq.empty())
                    {
                        State s = pq.top();
                        pq.pop();
                        if (s.lowerBound < upperBound)
                            temp.push_back(s);
                    }
                    for (int i = 0; i < temp.size(); ++i)
                        pq.push(temp[i]);
                }
            }
            continue;
        }
        for (int i = 0; i < graph.size(); ++i)
        {
            // 如果当前节点到下一个节点有边，且下一个节点未访问过，且当前花费加上当前节点到下一个节点的花费小于最小花费
            if (graph[state.current_city][i] != NO_EDGE && !state.vis[i] && graph[state.current_city][i] + state.now_cost < min_cost)
            {
                std::vector<int> newPath = state.path; // 新路径
                newPath.push_back(i);                  // 加入下一个节点
                std::vector<bool> newVis = state.vis;  // 新的标记数组
                newVis[i] = true;                      // 下一个节点标记为已访问
                double lowerBound = calculateLowerBound(graph, min2, newPath, newVis);
                if (lowerBound <= upperBound) // 如果下界小于等于上界，加入优先队列
                    pq.push(State(i, state.count + 1, state.now_cost + graph[state.current_city][i], lowerBound, newPath, newVis));
            }
        }
    }
}

void solve(const char *fileName, int start)
{
    std::cout << "filename: " << fileName << std::endl;                             // 输出文件名
    int n = 0, start_node = start;                                                  // 节点数，起始节点
    std::vector<std::vector<double>> graph;                                         // 邻接矩阵
    std::unordered_map<int, int> id2index;                                          // id到下标的映射
    std::vector<std::vector<double>> min2;                                          // 存储每个节点最短的2条路径的长度
    input(fileName, n, graph, id2index, min2);                                      // 读取输入文件
    std::vector<bool> vis(n, false);                                                // 标记节点是否被访问过
    vis[start_node] = true;                                                         // 起始节点标记为已访问
    double upperBound = calculateUpperBound(graph, vis, start_node, start_node, 1); // 计算上界
    // std::cout << "upperBound: " << upperBound << std::endl;                         // 输出上界

    long long sum_node = 0;                                      // 搜索节点数，初始化为0，longlong防止溢出
    double min_cost = NO_EDGE * n;                               // 最小花费
    std::vector<int> best_path(n + 1);                           // 最优路径，图总共n个点，回到起点又是一个点，所以是n+1个点
    vis[start_node] = true;                                      // 起始节点标记为已访问
    auto start_time = std::chrono::high_resolution_clock::now(); // 计时开始
    TSP(start_node, min_cost, best_path, graph, min2, sum_node, upperBound);
    auto end_time = std::chrono::high_resolution_clock::now();      // 计时结束
    std::chrono::duration<double> duration = end_time - start_time; // 计算耗时

    std::cout << "min_cost: " << min_cost << std::endl;
    std::cout << "best_path: ";
    for (int i = 0; i < best_path.size(); ++i)
        std::cout << id2index[best_path[i]] << " ";
    std::cout << std::endl
              << "time: " << duration.count() << "s" << std::endl;
    std::cout << "search node: " << sum_node << std::endl; // 输出搜索节点数
}

int main() // 使用了大量stl，编译时请使用"-O1"或更高级优化选项
{
    freopen("output2.txt", "w", stdout);             // 输出重定向到output.txt
    std::unordered_map<std::string, int> file2start; // 文件名到起始节点的映射
    file2start["15.txt"] = 12;
    file2start["20.txt"] = 17;
    file2start["22.txt"] = 19;
    file2start["30.txt"] = 19;

    solve("15.txt", file2start["15.txt"]);
    std::cout << std::endl;

    solve("20.txt", file2start["20.txt"]);
    std::cout << std::endl;

    solve("22.txt", file2start["22.txt"]);
    std::cout << std::endl;

    solve("30.txt", file2start["30.txt"]);
    std::cout << std::endl;
    fclose(stdout);
    return 0;
}

