#include "graph.h"

bool collison[N] = {0}; // 记录hash是否发生冲突
path dis1[N][N];        // 记录任意两点之间的距离
path dis2[N][N];        // 记录可采用骑车方式的最短距离
int tabu[N][N];         // 禁忌表
int tabulen[N][N];      // 禁忌期，最大值为5

void Graph::initVNode()
{
    ifstream file("node.txt");
    string line;

    if (file.is_open())
    {
        while (getline(file, line))
        {
            // 将有效名称提取出来
            int loc1 = line.find(' ');
            string name = line.substr(0, loc1);

            // 将有效坐标提取出来
            int loc2 = line.find('(');
            int x = 0, y = 0;
            // 读取x
            int flag = 1;
            while (line[++loc2] != ',')
            {
                if (line[loc2] == '-') // 特判负数
                {
                    flag = -1;
                    loc2++;
                }
                x = x * 10 + flag * (int)(line[loc2] - '0');
            }
            // 读取y
            flag = 1;
            while (line[++loc2] != ')')
            {
                if (line[loc2] == '-') // 特判负数
                {
                    flag = -1;
                    loc2++;
                }
                y = y * 10 + flag * (int)(line[loc2] - '0');
            }

            // 计算顶点名称字符串的hash函数值
            char c[50];
            strcpy(c, name.c_str()); // 将string转为char *
            unsigned int index = BKDRHash(c);
            // 线性探测
            while (collison[index])
            {
                index = (index + 1) % N;
            }
            collison[index] = true;

            // 将信息存入邻接表
            array[index].place_name = name;
            array[index].loc = make_pair(x, y);
            array[index].head = nullptr;
        }
        file.close();
    }
    else
        cout << "打开文件失败" << endl;
}

Graph::Graph()
{
    n = new Navigation();
    n->show();
    ifstream file("edge.txt");
    string line;
    array = new AdjList[N];
    initVNode();

    if (file.is_open())
    {
        while (getline(file, line))
        {
            // 处理每一行的字符串，提取出有用信息
            int loc1 = line.find(' ');
            int loc2 = line.find('~');
            int loc3 = line.find(' ', loc1 + 1);

            int flag;
            // 处理只可以步行的边
            if (line.find("only") != string::npos)
                flag = 1;
            else
                flag = 0;

            // 读取src地名
            string name1 = line.substr(0, loc1);
            // 读取dest地名
            string name2 = line.substr(loc2 + 1, loc3 - loc2 - 1);

            // 添加一条边
            addEdge(name1, name2, flag);
        }
        file.close();
    }
    else
        cout << "打开文件失败" << endl;
}

unsigned int Graph::getIndex(string name)
{
    char c[50];
    strcpy(c, name.c_str()); // 将string转为char *
    unsigned int index = BKDRHash(c);
    while (array[index].place_name != name)
    {
        index = (index + 1) % N;
    }
    return index;
}

void Graph::addEdge(string src, string dest, int flag)
{
    // 计算src对应的下标
    int index1 = getIndex(src);
    // 计算dest对应的下标
    int index2 = getIndex(dest);

    //  添加一条src到dest的边
    Node* newNode = new Node{index2, nullptr, flag};
    newNode->next = array[index1].head;
    array[index1].head = newNode;
}

void Graph::printGraph()
{
    for (int v = 0; v < N; ++v)
    {
        if (collison[v])
        {
            Node* pCrawl = array[v].head;
            cout << array[v].place_name << ":";
            while (pCrawl)
            {
                cout << " " << array[pCrawl->dest].place_name;
                pCrawl = pCrawl->next;
            }
            cout << endl;
        }
    }
}

unsigned int Graph::BKDRHash(char* str)
{
    unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;

    while (*str)
    {
        hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF) % N;
}

void Graph::printPath(unsigned int s, unsigned int d, int mode)
{
    if (mode == 0)
    {
        cout << "交通方式：步行" << endl;
        for (int i = 0, j = 1; i < dis1[s][d].cnt - 1; i = j - 1)
        {
            int xd = 0, yd = 0;
            while (j <= dis1[s][d].cnt - 1)
            {
                // x方向上的距离
                xd += array[dis1[s][d].s[j]].loc.x - array[dis1[s][d].s[j - 1]].loc.x;
                // y方向上的距离
                yd += array[dis1[s][d].s[j]].loc.y - array[dis1[s][d].s[j - 1]].loc.y;
                if (xd != 0 && yd != 0)
                {
                    xd -= array[dis1[s][d].s[j]].loc.x - array[dis1[s][d].s[j - 1]].loc.x;
                    yd -= array[dis1[s][d].s[j]].loc.y - array[dis1[s][d].s[j - 1]].loc.y;
                    break;
                }
                j++;
            }
            if (xd != 0)
            {
                if (xd > 0)
                    cout << "从 " << array[dis1[s][d].s[i]].place_name << " 向南走 " << xd << "米 "
                         << "到达 " << array[dis1[s][d].s[j - 1]].place_name << endl;
                        else
                        cout << "从 " << array[dis1[s][d].s[i]].place_name << " 向北走 " << -xd << "米 "
                         << "到达 " << array[dis1[s][d].s[j - 1]].place_name << endl;
            }
            else
            {
                if (yd > 0)
                    cout << "从 " << array[dis1[s][d].s[i]].place_name << " 向东走 " << yd << "米 "
                         << "到达 " << array[dis1[s][d].s[j - 1]].place_name << endl;
                        else
                        cout << "从 " << array[dis1[s][d].s[i]].place_name << " 向西走 " << -yd << " 米 "
                         << "到达 " << array[dis1[s][d].s[j - 1]].place_name << endl;
            }
        }
        cout << "到达目的地" << array[dis1[s][d].s[dis1[s][d].cnt - 1]].place_name << endl;
    }
    else
    {
        cout << "交通方式：步行+自行车" << endl;
        for (int i = 0; i < dis2[s][d].cnt - 1; i++)
        {
            // x方向上的距离
            int xd = array[dis2[s][d].s[i + 1]].loc.x - array[dis2[s][d].s[i]].loc.x;
            // y方向上的距离
            int yd = array[dis2[s][d].s[i + 1]].loc.y - array[dis2[s][d].s[i]].loc.y;
            if (xd != 0)
            {
                if (dis2[s][dis2[s][d].s[i + 1]].mode == 0)
                {
                    if (xd > 0)
                    {
                        cout << "从 " << array[dis2[s][d].s[i]].place_name << " 向南走 " << xd << "米 "
                             << "到达 " << array[dis2[s][d].s[i + 1]].place_name << endl;
                                    cout << "拥塞程度为：" << dis2[s][dis2[s][d].s[i + 1]].congest << endl;
                    }
                    else
                    {
                        cout << "从 " << array[dis2[s][d].s[i]].place_name << " 向北走 " << -xd << "米 "
                             << "到达 " << array[dis2[s][d].s[i + 1]].place_name << endl;
                                    cout << "拥塞程度为：" << dis2[s][dis2[s][d].s[i + 1]].congest << endl;
                    }
                }
                else
                {
                    if (xd > 0)
                    {
                        cout << "从 " << array[dis2[s][d].s[i]].place_name << " 向南骑自行车 " << xd << "米 "
                             << "到达 " << array[dis2[s][d].s[i + 1]].place_name << endl;
                                    cout << "拥塞程度为：" << dis2[s][dis2[s][d].s[i + 1]].congest << endl;
                    }
                    else
                    {
                        cout << "从 " << array[dis2[s][d].s[i]].place_name << " 向北骑自行车 " << -xd << "米 "
                             << "到达 " << array[dis2[s][d].s[i + 1]].place_name << endl;
                                    cout << "拥塞程度为：" << dis2[s][dis2[s][d].s[i + 1]].congest << endl;
                    }
                }
            }
            else
            {
                if (dis2[s][dis2[s][d].s[i + 1]].mode == 0)
                {
                    if (yd > 0)
                    {
                        cout << "从 " << array[dis2[s][d].s[i]].place_name << " 向东走 " << yd << "米 "
                             << "到达 " << array[dis2[s][d].s[i + 1]].place_name << endl;
                                    cout << "拥塞程度为：" << dis2[s][dis2[s][d].s[i + 1]].congest << endl;
                    }
                    else
                    {
                        cout << "从 " << array[dis2[s][d].s[i]].place_name << " 向西走 " << -yd << " 米 "
                             << "到达 " << array[dis2[s][d].s[i + 1]].place_name << endl;
                                    cout << "拥塞程度为：" << dis2[s][dis2[s][d].s[i + 1]].congest << endl;
                    }
                }
                else
                {
                    if (yd > 0)
                    {
                        cout << "从 " << array[dis2[s][d].s[i]].place_name << " 向东骑自行车 " << yd << "米 "
                             << "到达 " << array[dis2[s][d].s[i + 1]].place_name << endl;
                                    cout << "拥塞程度为：" << dis2[s][dis2[s][d].s[i + 1]].congest << endl;
                    }
                    else
                    {
                        cout << "从 " << array[dis2[s][d].s[i]].place_name << " 向西骑自行车 " << -yd << " 米 "
                            << "到达 " << array[dis2[s][d].s[i + 1]].place_name << endl;
                                       cout << "拥塞程度为：" << dis2[s][dis2[s][d].s[i + 1]].congest << endl;
                    }
                }
            }
        }
        cout << "到达目的地" << array[dis2[s][d].s[dis2[s][d].cnt - 1]].place_name << endl;
    }
}

void Graph::getPath(unsigned int s, int* prev, int* transportation, int* temp_dist, int mode, double* congestion)
{
    // 修改:双向建立最短路径
    if (mode == 0)
    {
        for (int i = 0; i < N; i++)
        {
            if (dis1[s][i].cnt) // 已经求出最短路径无需再求
                continue;
            dis1[s][i].dist = temp_dist[i];
            dis1[s][i].mode = transportation[i];
            dis1[i][s].mode = transportation[i];
            dis1[s][i].congest = congestion[i];
            dis1[i][s].congest = congestion[i];
            int temp = i;
            while (temp != -1)
            {
                dis1[s][i].s[dis1[s][i].cnt++] = temp;
                temp = prev[temp];
            }

            // 双向建立最短路径
            memcpy(dis1[i][s].s, dis1[s][i].s, sizeof(dis1[s][i].s));
            dis1[i][s].dist = dis1[s][i].dist;
            dis1[i][s].cnt = dis1[s][i].cnt;

            // 翻转路径
            reverse(dis1[s][i].s, dis1[s][i].s + dis1[s][i].cnt);
        }
    }
    else
    {
        for (int i = 0; i < N; i++)
        {
            if (dis2[s][i].cnt) // 已经求出最短路径无需再求
                continue;
            dis2[s][i].dist = temp_dist[i];
            dis2[s][i].mode = transportation[i];
            dis2[i][s].mode = transportation[i];
            dis2[s][i].congest = congestion[i];
            dis2[i][s].congest = congestion[i];
            int temp = i;
            while (temp != -1)
            {
                dis2[s][i].s[dis2[s][i].cnt++] = temp;
                temp = prev[temp];
            }

            // 双向建立最短路径
            memcpy(dis2[i][s].s, dis2[s][i].s, sizeof(dis2[s][i].s));
            dis2[i][s].dist = dis2[s][i].dist;
            dis2[i][s].cnt = dis2[s][i].cnt;

            // 翻转路径
            reverse(dis2[s][i].s, dis2[s][i].s + dis2[s][i].cnt);
        }
    }
}

void Graph::dijkstra(string src, string dest, int mode)
{
    unsigned int s = getIndex(src);
    unsigned int d = getIndex(dest);
    if ((mode == 0 && dis1[s][d].cnt) || (mode == 1 && dis2[s][d].cnt)) // 若已经求得最短路径则直接输出
    {
        // printPath(s, d);
        return;
    }

    // 小根堆优先队列
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; // 小根堆维护距离
    pq.push(make_pair(0, s));                                                           // 将起点入队

    int temp_dis[N];                         // 存放源点到各个结点的最短距离
    memset(temp_dis, INF, sizeof(temp_dis)); // 初始化距离为INF
    temp_dis[s] = 0;                         // 到自己的距离为0

    int visit[N];                    // 记录是否已经找到到i的最短距离
    memset(visit, 0, sizeof(visit)); // 初始化为没有找到

    int prev[N];                    // 记录最短路径的前驱节点,
    int transportation[N];          // 交通方式
    double congestion[N];           // 拥塞程度
    memset(prev, -1, sizeof(prev)); // 每个节点的前驱节点初始化为-1
    memset(transportation, 0, sizeof(transportation));
    memset(congestion, 0, sizeof(congestion));

    while (!pq.empty())
    {
        //   堆不空
        int u = pq.top().y;
        visit[u] = 1;
        pq.pop();
        Node* temp = array[u].head;
        default_random_engine e;
        uniform_int_distribution<int> d(50, 500);
        e.seed(time(0));

        while (temp != nullptr)
        {
            //  枚举所有出边
            int v = temp->dest;
            double w = abs(array[u].loc.x - array[v].loc.x) + abs(array[u].loc.y - array[v].loc.y);

            int is_take_bicycle = 0; // 是否采用骑自行车的方式

            // 随机产生一个0.5-5的浮点数
            int cgs = d(e);
            double congst = cgs / 100.0; // 随机产生的道路拥塞量

            if (mode == 1 && temp->flag == 0)
            {
                if ((w / 3 * congst)  < w)
                {
                    w = w / 3 * congst;
                    is_take_bicycle = 1;
                }
            }

            if (!visit[v] && temp_dis[v] > temp_dis[u] + w)
            {
                // 松弛操作
                temp_dis[v] = temp_dis[u] + w;
                prev[v] = u;
                congestion[v] = congst;
                transportation[v] = is_take_bicycle;
                pq.push(make_pair(temp_dis[v], v));
            }
            temp = temp->next;
        }
    }

    // 找出s到其余所有点的最短路径
    getPath(s, prev, transportation, temp_dis, mode, congestion);

    // printPath(s, d);
    return;
}

Graph::~Graph()
{
    delete[] array;
    delete n;
}

int Graph::calcPathLen(const vector<int>& path, int mode)
{
    int res = 0;
    if (mode == 0)
    {
        for (int i = 0; i < path.size() - 1; i++)
        {
            dijkstra(array[path[i]].place_name, array[path[i + 1]].place_name, mode);
            res += dis1[path[i]][path[i + 1]].dist;
        }

        dijkstra(array[path[path.size() - 1]].place_name, array[path[0]].place_name, mode);
        res += dis1[path[path.size() - 1]][path[0]].dist;
    }
    else
    {
        for (int i = 0; i < path.size() - 1; i++)
        {
            dijkstra(array[path[i]].place_name, array[path[i + 1]].place_name, mode);
            res += dis2[path[i]][path[i + 1]].dist;
        }

        dijkstra(array[path[path.size() - 1]].place_name, array[path[0]].place_name, mode);
        res += dis2[path[path.size() - 1]][path[0]].dist;
    }
    return res;
}

void Graph::swapVal(int& a, int& b)
{
    int tmp = a;
    a = b;
    b = tmp;
}

vector<int> Graph::generateCandidate(const vector<int>& cur_path)
{
    vector<int> candidate(cur_path);
    // srand(unsigned(time(0))); // 更改随机种子
    int x = rand() % cur_path.size();
    int y = rand() % cur_path.size();
    if (x == 0)
        x++;
    if (y == 0)
        y++;
    while (x == y)
    {
        y = rand() % cur_path.size();
        if (y == 0)
            y++;
    }
    swapVal(candidate[x], candidate[y]);
    return candidate;
}

bool Graph::isInTabuList(const vector<int>& path)
{
    for (int i = 1; i < path.size(); i++)
    {
        if (tabu[path[i - 1]][path[i]])
        {
            return false;
        }
    }
    if (tabu[path[path.size() - 1]][path[0]])
    {
        return false;
    }
    return true;
}

void Graph::updateTabuList(const vector<int>& cur_path, const vector<int>& new_path)
{
    // tabulen_max=5
    for (int i = 1; i < cur_path.size(); i++)
    {
        tabulen[cur_path[i - 1]][cur_path[i]]--;
        if (tabulen[cur_path[i - 1]][cur_path[i]] <= 0)
        {
            tabu[cur_path[i - 1]][cur_path[i]] = 0;
        }
        tabulen[new_path[i - 1]][new_path[i]] = 5;
        tabu[new_path[i - 1]][new_path[i]] = 1;
    }
    tabulen[cur_path[cur_path.size() - 1]][cur_path[0]]--;
    if (tabulen[cur_path[cur_path.size() - 1]][cur_path[0]] <= 0)
    {
        tabu[cur_path[cur_path.size() - 1]][cur_path[0]] = 0;
    }
    tabulen[new_path[cur_path.size() - 1]][new_path[0]] = 5;
    tabu[new_path[cur_path.size() - 1]][new_path[0]] = 1;
}

vector<int> Graph::tabuSearch(string src, vector<string> dst, int mode)
{
    // 初始化当前解
    vector<int> cur_path;
    cur_path.push_back(getIndex(src));
    for (int i = 0; i < dst.size(); i++)
    {
        cur_path.push_back(getIndex(dst[i]));
    }

    // 随机生成一个新排列
    // srand(unsigned(time(0))); // 更改随机种子
    random_shuffle(cur_path.begin() + 1, cur_path.end());
    vector<int> best_path = cur_path;

    // 初始化禁忌表和禁忌期
    memset(tabu, 0, sizeof(tabu));
    memset(tabulen, 0, sizeof(tabulen));

    // 设置算法参数
    int maxiter = 5000;

    // 迭代搜索
    for (int iter = 0; iter < maxiter; iter++)
    {
        // 生成候选解并选择其中的一个最优解
        vector<int> new_path;
        int cnt = 0;
        while (cnt < 500)
        {
            new_path = generateCandidate(cur_path);
            if (!isInTabuList(new_path))
            {
                break;
            }
            cnt++;

            // 更新禁忌表和禁忌期
            updateTabuList(cur_path, new_path);
        }

        // 更新禁忌表和禁忌期
        updateTabuList(cur_path, new_path);

        // 更新当前解和最优解
        cur_path = new_path;
        if (calcPathLen(cur_path, mode) < calcPathLen(best_path, mode))
        {
            best_path = cur_path;
        }
    }

    // 返回最优路径
    cout << calcPathLen(best_path, mode);
    return best_path;

    // 输出最优路径
    //    for (int i = 0; i < best_path.size(); i++)
    //    {
    //        cout << array[best_path[i]].place_name << endl;
    //    }

    //    for (int i = 0; i < best_path.size() - 1; i++)
    //    {
    //        cout << array[best_path[i]].place_name << endl;
    //        printPath(best_path[i], best_path[i + 1], mode);
    //    }

    //    cout << array[best_path[best_path.size() - 1]].place_name << " " << array[best_path[0]].place_name << endl;
    //    printPath(best_path[best_path.size() - 1], best_path[0], mode);
}

// 利用dfs算法求解TSP问题
void Graph::dfs(vector<int>& nums, vector<int>& visited, vector<int>& permutation, vector<int>& best_path, int& lowest_cost, int mode)
{
    if (permutation.size() == nums.size() + 1)
    {
        // 判断是否找到了一个更优解
        if(calcPathLen(permutation, mode) < lowest_cost)
        {
            best_path = permutation;
            lowest_cost = calcPathLen(permutation, mode);
        }
        //cout << lowest_cost << endl;
        return;
    }

    for (int i = 0; i < nums.size(); i++)
    {
        if (!visited[i])
        {
            visited[i] = 1;
            permutation.push_back(nums[i]);

            // 递归搜索
            dfs(nums, visited, permutation, best_path, lowest_cost, mode);

            // 回溯
            permutation.pop_back();
            visited[i] = 0;
        }
    }
}

vector<int> Graph::dfsTsp(string src, vector<string> dst, int mode)
{
    // 初始化当前解
    vector<int> cur_path;
    for (int i = 0; i < dst.size(); i++)
    {
        cur_path.push_back(getIndex(dst[i]));
    }

    vector<int> visited(cur_path.size()), permutation, best_path;
    // 起点下标
    int s = getIndex(src);
    permutation.push_back(s);
    int lowest_cost = INT_MAX;
    dfs(cur_path, visited, permutation, best_path, lowest_cost, mode);
    cout << lowest_cost << endl;
    return best_path;
}

void Graph::show_dijk(string src, string dest, int mode)
{
    dijkstra(src, dest, mode);
    int s = getIndex(src);
    int d = getIndex(dest);
    if(mode == 0)
    {
        for(int i = 0; i < dis1[s][d].cnt - 1; i++)
        {
            int x1 = array[dis1[s][d].s[i]].loc.first;
            int y1 = array[dis1[s][d].s[i]].loc.second;
            int x2 = array[dis1[s][d].s[i + 1]].loc.first;
            int y2 = array[dis1[s][d].s[i + 1]].loc.second;
            n->DrawLine(QLine(QPoint(x1, y1), QPoint(x2, y2)), (abs(x1 - x2 + y1 - y2)) / 30.00, 0, dis1[s][dis1[s][d].s[i + 1]].congest);
        }
    }
    else
    {
        for(int i = 0; i < dis2[s][d].cnt - 1; i++)
        {
            int x1 = array[dis2[s][d].s[i]].loc.first;
            int y1 = array[dis2[s][d].s[i]].loc.second;
            int x2 = array[dis2[s][d].s[i + 1]].loc.first;
            int y2 = array[dis2[s][d].s[i + 1]].loc.second;
            int transportation;
            int alpha;// 加速比
            if(dis2[s][dis2[s][d].s[i + 1]].mode == 1)
            {
                transportation = 1;
                alpha = 3 / dis2[s][dis2[s][d].s[i + 1]].congest;
            }

            else
            {
                transportation = 0;
                alpha = 1;
            }
            n->DrawLine(QLine(QPoint(x1, y1), QPoint(x2, y2)), (abs(x1 - x2 + y1 - y2)) / 30.00 / alpha, transportation, dis2[s][dis2[s][d].s[i + 1]].congest);
        }
    }
}

void Graph::show_dfs(string src, vector<string> dst, int mode)
{
    int s, d;
    vector<int> best_path = tabuSearch(src, dst, mode);
    for(int j = 0; j < best_path.size(); j++)
    {
        if(j == best_path.size() - 1)
        {
            s = best_path[j];
            d = best_path[0];
        }
        else
        {
            s = best_path[j];
            d = best_path[j + 1];
        }
        qDebug() << QString::fromStdString(array[s].place_name) << QString::fromStdString(array[d].place_name);
        if(mode == 0)
        {
            for(int i = 0; i < dis1[s][d].cnt - 1; i++)
            {
                int x1 = array[dis1[s][d].s[i]].loc.first;
                int y1 = array[dis1[s][d].s[i]].loc.second;
                int x2 = array[dis1[s][d].s[i + 1]].loc.first;
                int y2 = array[dis1[s][d].s[i + 1]].loc.second;
                n->DrawLine(QLine(QPoint(x1, y1), QPoint(x2, y2)), (abs(x1 - x2 + y1 - y2)) / 30.00, 0, dis1[s][dis1[s][d].s[i + 1]].congest);
            }
        }
        else
        {
            for(int i = 0; i < dis2[s][d].cnt - 1; i++)
            {
                int x1 = array[dis2[s][d].s[i]].loc.first;
                int y1 = array[dis2[s][d].s[i]].loc.second;
                int x2 = array[dis2[s][d].s[i + 1]].loc.first;
                int y2 = array[dis2[s][d].s[i + 1]].loc.second;
                int transportation;// 交通方式
                int alpha;// 加速比
                if(dis2[s][dis2[s][d].s[i + 1]].mode == 1)
                {
                    transportation = 1;
                    alpha = 3 / dis2[s][dis2[s][d].s[i + 1]].congest;
                }

                else
                {
                    transportation = 0;
                    alpha = 1;
                }
                n->DrawLine(QLine(QPoint(x1, y1), QPoint(x2, y2)), (abs(x1 - x2 + y1 - y2)) / 30.00 / alpha, transportation, dis2[s][dis2[s][d].s[i + 1]].congest);
            }
        }
    }
}

void Graph::show_tabu(string src, vector<string> dst, int mode)
{
    int s, d;
    vector<int> best_path = dfsTsp(src, dst, mode);
    for(int j = 0; j < best_path.size(); j++)
    {
        if(j == best_path.size() - 1)
        {
            s = best_path[j];
            d = best_path[0];
        }
        else
        {
            s = best_path[j];
            d = best_path[j + 1];
        }
        qDebug() << QString::fromStdString(array[s].place_name) << QString::fromStdString(array[d].place_name);
        if(mode == 0)
        {
            for(int i = 0; i < dis1[s][d].cnt - 1; i++)
            {
                int x1 = array[dis1[s][d].s[i]].loc.first;
                int y1 = array[dis1[s][d].s[i]].loc.second;
                int x2 = array[dis1[s][d].s[i + 1]].loc.first;
                int y2 = array[dis1[s][d].s[i + 1]].loc.second;
                n->DrawLine(QLine(QPoint(x1, y1), QPoint(x2, y2)), (abs(x1 - x2 + y1 - y2)) / 30.00, 0, dis1[s][dis1[s][d].s[i + 1]].congest);
            }
        }
        else
        {
            for(int i = 0; i < dis2[s][d].cnt - 1; i++)
            {
                int x1 = array[dis2[s][d].s[i]].loc.first;
                int y1 = array[dis2[s][d].s[i]].loc.second;
                int x2 = array[dis2[s][d].s[i + 1]].loc.first;
                int y2 = array[dis2[s][d].s[i + 1]].loc.second;
                int transportation;
                int alpha;// 加速比
                if(dis2[s][dis2[s][d].s[i + 1]].mode == 1)
                {
                    transportation = 1;
                    alpha = 3 / dis2[s][dis2[s][d].s[i + 1]].congest;
                }

                else
                {
                    transportation = 0;
                    alpha = 1;
                }
                n->DrawLine(QLine(QPoint(x1, y1), QPoint(x2, y2)), (abs(x1 - x2 + y1 - y2)) / 30.00 / alpha, transportation, dis2[s][dis2[s][d].s[i + 1]].congest);
            }
        }
    }
}
