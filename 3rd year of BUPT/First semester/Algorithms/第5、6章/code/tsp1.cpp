#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <stack>
#include <chrono>

#define NO_EDGE 99999

/*
 * fileName: 输入文件名
 * n: 节点数
 * graph: 邻接矩阵
 * id2index: id到下标的映射
 */
void input(const char *fileName, int &n, std::vector<std::vector<double>> &graph, std::unordered_map<int, int> &id2index)
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
        }
    }

    inputFile.close();
}

/*
 * count: 当前已经访问的节点数
 * current_city: 当前所在的节点
 * start_city: 起始节点
 * min_cost: 最小花费
 * now_cost: 当前花费
 * vis: 标记是否访问过
 * graph: 邻接矩阵
 * sum_node: 搜索节点数
 */
void TSP(int count, int current_city, int start_city, double &min_cost, double now_cost, std::vector<int> &best_path, std::vector<int> &path, std::vector<bool> &vis, std::vector<std::vector<double>> &graph, long long &sum_node)
{
    sum_node++;                // 搜索节点数加1
    if (count == graph.size()) // 所有节点都访问过了，回到起始节点
    {
        // 如果当前节点到起始节点有边，且当前花费加上当前节点到起始节点的花费小于最小花费
        if (graph[current_city][start_city] != NO_EDGE && now_cost + graph[current_city][start_city] < min_cost)
        {
            min_cost = now_cost + graph[current_city][start_city];
            best_path = path;
            best_path.push_back(start_city);
        }
        return;
    }

    for (int i = 0; i < graph.size(); ++i)
    {
        // 如果当前节点到下一个节点有边，且下一个节点未访问过，且当前花费加上当前节点到下一个节点的花费小于最小花费
        if (graph[current_city][i] != NO_EDGE && !vis[i] && graph[current_city][i] + now_cost < min_cost)
        {
            vis[i] = true;     // 标记为已访问
            path.push_back(i); // 加入路径
            TSP(count + 1, i, start_city, min_cost, now_cost + graph[current_city][i], best_path, path, vis, graph, sum_node);
            path.pop_back(); // 回溯
            vis[i] = false;
        }
    }
}

struct State // 栈中状态，引用变量不用拷贝
{
    int count;
    int current_city;
    int start_city;
    double now_cost;
    int last_index; // 上一个节点在path中的下标
    bool back_flag; // 是否是回溯
    // 构造函数
    State(int count, int current_city, int start_city, double now_cost, int last_index, bool back_flag) : count(count), current_city(current_city), start_city(start_city), now_cost(now_cost), last_index(last_index), back_flag(back_flag) {}
};
/*
 * count: 当前已经访问的节点数
 * current_city: 当前所在的节点
 * start_city: 起始节点
 * min_cost: 最小花费
 * now_cost: 当前花费
 * vis: 标记是否访问过
 * graph: 邻接矩阵
 */
long long TSP_stack(int count, int current_city, int start_city, double &min_cost, double now_cost, std::vector<int> &best_path, std::vector<int> &path, std::vector<bool> &vis, std::vector<std::vector<double>> &graph)
{
    long long sum_node = 0; // 搜索节点数
    std::stack<State, std::vector<State>> state_stack;
    state_stack.push(State(count, current_city, start_city, now_cost, 0, false));
    sum_node++;
    while (!state_stack.empty())
    {
        State &state = state_stack.top();
        switch (state.back_flag)
        {
        case false: // 前进
        {
            if (state.count == graph.size())
            {
                if (graph[state.current_city][state.start_city] != NO_EDGE && state.now_cost + graph[state.current_city][state.start_city] < min_cost)
                {
                    min_cost = state.now_cost + graph[state.current_city][state.start_city];
                    best_path = path;
                    best_path.push_back(state.start_city);
                }
                state_stack.top().back_flag = true;
                break;
            }
            bool forward_flag = false;
            for (int i = 0; i < graph.size(); ++i)
            {
                if (graph[state.current_city][i] != NO_EDGE && !vis[i] && graph[state.current_city][i] + state.now_cost < min_cost)
                {
                    vis[i] = true;     // 标记为已访问
                    path.push_back(i); // 加入路径
                    state_stack.push(State(state.count + 1, i, start_city, state.now_cost + graph[state.current_city][i], i, false));
                    forward_flag = true; // 有前进
                    sum_node++;          // 搜索节点数加1
                    break;
                }
            }
            if (!forward_flag) // 没有前进，回溯
                state_stack.top().back_flag = true;
            break;
        }
        case true: // 回溯
        {
            int last_index = state.last_index; // 上一个节点在path中的下标
            vis[last_index] = false;           // 标记为未访问
            path.pop_back();                   // 从路径中删除
            state_stack.pop();                 // 从栈中删除
            if (state_stack.empty())           // 栈空，退出
                break;
            state = state_stack.top(); // state维护的是栈顶元素的引用，所以要更新state
            bool forward_flag = false;
            for (int i = last_index + 1; i < graph.size(); ++i) // 从上一个节点的下一个节点开始找
            {
                if (graph[state.current_city][i] != NO_EDGE && !vis[i] && graph[state.current_city][i] + state.now_cost < min_cost)
                {
                    vis[i] = true;     // 标记为已访问
                    path.push_back(i); // 加入路径
                    state_stack.push(State(state.count + 1, i, start_city, state.now_cost + graph[state.current_city][i], i, false));
                    forward_flag = true; // 有前进
                    sum_node++;          // 搜索节点数加1
                    break;
                }
            }
            if (!forward_flag) // 没有前进，回溯
                state_stack.top().back_flag = true;
        }
        }
    }
    return sum_node; // 返回搜索节点数
}

// fileName: 输入文件名
void solve(const char *fileName, int start)
{
    std::cout << "filename: " << fileName << std::endl; // 输出文件名
    int n = 0;                                          // 节点数
    std::vector<std::vector<double>> graph;             // 邻接矩阵
    std::unordered_map<int, int> id2index;              // id到下标的映射
    input(fileName, n, graph, id2index);                // 读取输入文件
    double min_cost = NO_EDGE * n;                      // 最小花费
    std::vector<int> best_path(n + 1);                  // 最优路径，图总共n个点，回到起点又是一个点，所以是n+1个点
    int start_node = start;                             // 起始节点
    std::vector<int> path;                              // 当前路径
    std::vector<bool> vis(n, false);                    // 标记是否访问过
    vis[start_node] = true;                             // 起始节点标记为已访问
    path.push_back(start_node);                         // 起始节点加入路径

    long long sum_node = 0;                                      // 搜索节点数，初始化为0，longlong防止溢出
    auto start_time = std::chrono::high_resolution_clock::now(); // 计时开始
    // TSP(1, start_node, start_node, min_cost, 0, best_path, path, vis, graph, sum_node);
    sum_node = TSP_stack(1, start_node, start_node, min_cost, 0, best_path, path, vis, graph);
    auto end_time = std::chrono::high_resolution_clock::now();      // 计时结束
    std::chrono::duration<double> duration = end_time - start_time; // 计算耗时

    std::cout << "min_cost: " << min_cost << std::endl;
    std::cout << "best_path: ";
    for (int i = 0; i < best_path.size(); ++i) // 输出最短路径
    {
        std::cout << id2index[best_path[i]] << " "; // 输出id
        // std::cout << best_path[i] << " ";           // 输出下标
    }
    std::cout << std::endl
              << "time: " << duration.count() << "s" << std::endl;
    std::cout << "search node: " << sum_node << std::endl; // 输出搜索节点数
}

int main() // 由于用了大量stl，编译时请使用"-O1"或更高级优化选项
{
    freopen("output1.txt", "w", stdout);             // 将输出重定向到output.txt
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

