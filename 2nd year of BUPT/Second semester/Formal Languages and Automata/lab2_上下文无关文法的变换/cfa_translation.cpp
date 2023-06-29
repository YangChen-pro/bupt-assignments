#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <bitset>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <string>
using namespace std;

unordered_map<char, vector<vector<char>>> P; // 产生式集合
set<char> N;                                 // 非终结符集合
set<char> T;                                 // 终结符集合
char S;                                      // 起始符号


// 从文件 input.txt 中读取文法规则
void read_input()
{
    //ifstream f("input.txt"); // 打开文件

    string input;
    getline(cin, input); // 读取第一行，即非终结符集合

    // 解析非终结符集合
    size_t pos = input.find("N={"), end_pos = input.find("}");
    for (size_t i = pos + 3; i < end_pos; i++)
        if (input[i] != ',' && input[i] != ' ')
            N.insert(input[i]); // 将非终结符加入集合 N 中

    // getline(f, input);
    getline(cin, input); // 读取第二行，即终结符集合

    // 解析终结符集合
    pos = input.find("T={"), end_pos = input.find("}");
    for (size_t i = pos + 3; i < end_pos; i++)
        if (input[i] != ',' && input[i] != ' ')
            T.insert(input[i]); // 将终结符加入集合 T 中

    // getline(f, input);
    getline(cin, input); // 读取第三行，跳过

    // 解析产生式集合
    // while (getline(f, input))
    while (getline(cin, input))
    {
        if (input[0] == 'S')
            break; // 已经读取完所有的产生式

        char key = input[1];                  // 产生式的左部非终结符
        size_t arrow_pos = input.find("-->"); // 产生式的箭头位置
        vector<vector<char>> value;           // 产生式的右部，可能有多个可能性
        size_t start_pos = arrow_pos + 3;     // 产生式右部第一个字符的位置

        // 解析产生式右部
        while (start_pos < input.size())
        {
            size_t end_pos = input.find("|", start_pos); // 找到下一个可能的右部
            if (end_pos == string::npos)
                end_pos = input.size(); // 如果没有找到，说明这是最后一个右部

            vector<char> rhs; // 一个右部
            for (size_t i = start_pos; i < end_pos; i++)
                if (input[i] != ' ')
                    rhs.push_back(input[i]); // 将右部加入 rhs 中

            value.push_back(rhs);    // 将 rhs 加入 value 中
            start_pos = end_pos + 1; // 找下一个右部
        }

        P[key] = value; // 将产生式加入 P 中
    }

    pos = input.find("S="), S = input[pos + 2]; // 读取起始符号
    //f.close();                                  // 关闭文件
}

// 输出文法规则
void print_grammar()
{
    cout << "N={";
    for (auto it = N.begin(); it != N.end(); ++it)
    {
        cout << *it;
        if (std::next(it) != N.end()) // 如果不是最后一个元素，输出逗号
            cout << ",";
    }
    cout << "}" << endl;

    cout << "T={";
    for (auto it = T.begin(); it != T.end(); ++it)
    {
        cout << *it;
        if (std::next(it) != T.end()) // 如果不是最后一个元素，输出逗号
            cout << ",";
    }
    cout << "}" << endl;

    cout << "P:" << endl;
    // 将 P 按照 key 排序
    map<char, vector<vector<char>>> sorted_P(P.begin(), P.end()); 
    for (auto &[key, value] : sorted_P)
    {
        cout << "\t" << key << "-->";
        set<string> sorted_value; // 将 value 按照字典序排序
        for (auto it = value.begin(); it != value.end(); ++it)
        {
            string str(it->begin(), it->end());
            sorted_value.insert(str);
        }
        for (auto it = sorted_value.cbegin(); it != sorted_value.cend(); ++it)
        {
            string s = *it;
            for (auto c : s)
                cout << c;
            if (std::next(it) != sorted_value.cend())
                cout << "|";
        }
        cout << endl;
    }

    cout << "S=" << S << endl;
}

// 消除ε产生式
void eliminate_epsilon()
{
    //cout << "消去epsilon(ε)产生式" << endl;

    // 第一步，找到所有可以直接推出 ε 的非终结符
    unordered_set<char> eps_generating_nonterminals;
    for (auto &[key, value] : P)
    {
        for (auto &rhs : value)
        {
            if (rhs.size() == 1 && rhs[0] == 'N')
            {
                eps_generating_nonterminals.insert(key);
                break;
            }
        }
    }

    // 第二步，不断找到新的可以推出 ε 的非终结符，直到集合不再变化
    while (true)
    {
        auto old_size = eps_generating_nonterminals.size();
        for (auto &[key, value] : P)
        {
            if (eps_generating_nonterminals.count(key) > 0) // 如果 key 已经在集合中，跳过
                continue;
            for (auto &rhs : value)
            {
                bool can_generate_eps = true;
                for (auto &sym : rhs)
                {
                    if (eps_generating_nonterminals.count(sym) == 0) // 如果 rhs 中有一个符号不能推出 ε，那么 key 也不能推出 ε
                    {
                        can_generate_eps = false;
                        break;
                    }
                }
                if (can_generate_eps) // 如果 rhs 中的所有符号都可以推出 ε，那么 key 也可以推出 ε
                {
                    eps_generating_nonterminals.insert(key);
                    break;
                }
            }
        }
        if (eps_generating_nonterminals.size() == old_size) // 如果集合不再变化，跳出循环
            break;
    }

    // 第三步，根据新的产生式集合生成新的产生式集合 P1
    unordered_map<char, vector<vector<char>>> P1; // 新的产生式集合 P1
    for (auto &[key, value] : P)                  // 遍历原始产生式集合 P 中的每个非终结符 key 和其对应的产生式列表 value
    {
        vector<vector<char>> new_rhs_list{}; // 新的产生式列表
        for (auto &rhs : value)              // 遍历产生式列表 value 中的每个产生式 rhs
        {
            int total_in_nonterminals = 0; // 记录产生式 rhs 中可以推出空串的非终结符个数
            for (auto &i : rhs)
            {
                if (eps_generating_nonterminals.count(i) > 0) // 如果当前符号可以推出空串
                    total_in_nonterminals++;                  // 非终结符个数加一
            }

            for (int num = 0; num < (1 << total_in_nonterminals); num++) // 枚举可以推出空串的非终结符的所有可能组合
            {
                vector<char> new_rhs{};      // 新的产生式
                int num_in_nonterminals = 0; // 当前处理的可以推出空串的非终结符的编号
                for (auto &i : rhs)          // 遍历产生式 rhs 中的每个符号 i
                {
                    if (eps_generating_nonterminals.count(i) > 0) // 如果当前符号可以推出空串
                    {
                        if ((num >> num_in_nonterminals) & 1) // 如果当前非终结符在当前组合中被选中
                            new_rhs.push_back(i);             // 将当前非终结符加入新的产生式中
                        num_in_nonterminals++;                // 非终结符编号加一
                    }
                    else
                        new_rhs.push_back(i); // 将终结符直接加入新的产生式中
                }
                if (!new_rhs.empty() && new_rhs[0] != 'N') // 如果新的产生式非空且不是空串产生式
                    new_rhs_list.push_back(new_rhs);       // 将新的产生式加入新的产生式列表中
            }
        }
        sort(new_rhs_list.begin(), new_rhs_list.end());                                           // 对新的产生式列表进行排序
        new_rhs_list.erase(unique(new_rhs_list.begin(), new_rhs_list.end()), new_rhs_list.end()); // 去除重复的产生式
        P1[key] = new_rhs_list;                                                                   // 将新的产生式列表加入新的产生式集合 P1 中
    }

    // 如果起始符号可以推出 ε，则需要增加一个新的起始符号
    if (eps_generating_nonterminals.count(S) > 0)
    {
        char new_S = 'T';
        P1[new_S] = {{S}, {'N'}};
        S = new_S;
        N.insert(S);
    }

    P = move(P1); // 用新的产生式集合 P1 替换原来的产生式集合 P
}

// 消去单产生式
void eliminate_single()
{
    //cout << "消去单产生式" << endl;

    // 定义新的非终结符集合、新的产生式集合和新的起始符号
    unordered_set<char> new_nonterminals{};                      // 新的非终结符集合
    unordered_map<char, vector<vector<char>>> new_productions{}; // 新的产生式集合
    char new_start = S;                                          // 新的起始符号

    // 遍历原始产生式集合 P 中的每个非终结符 A
    for (auto A : N)
    {
        // 初始化新的产生式列表和 new_nonterminals
        new_productions[A] = {};
        new_nonterminals.clear();
        new_nonterminals.insert(A);

        // 找出所有可以直接推出单个非终结符的产生式，并将这些非终结符加入 new_nonterminals 中
        for (auto rhs : P[A])
            if (rhs.size() == 1 && N.count(rhs[0]) > 0)
                new_nonterminals.insert(rhs[0]);

        // 不断扩展 new_nonterminals，直到 new_nonterminals 不再变化为止
        unordered_set<char> prev_nonterminals{};
        do
        {
            prev_nonterminals = new_nonterminals;
            for (auto B : prev_nonterminals)
                for (auto rhs : P[B])
                    if (rhs.size() == 1 && N.count(rhs[0]) > 0) // 如果当前产生式可以直接推出单个非终结符
                        new_nonterminals.insert(rhs[0]);
        } while (new_nonterminals != prev_nonterminals);

        // 构建新的产生式列表
        for (auto B : new_nonterminals)
            for (auto rhs : P[B])
                if (rhs.size() != 1 || T.count(rhs[0]) > 0 || rhs[0] == 'N') // 如果当前产生式不是单个终结符或者空串
                    new_productions[A].push_back(rhs);

        // 去除重复的产生式，如果新的产生式列表为空，则添加一个产生式 A -> N
        sort(new_productions[A].begin(), new_productions[A].end());
        new_productions[A].erase(unique(new_productions[A].begin(), new_productions[A].end()), new_productions[A].end());
        if (new_productions[A].empty())
            new_productions[A].push_back({'N'});

        // 如果当前非终结符是原始文法的起始符号，则更新新的起始符号
        if (A == S)
            new_start = new_nonterminals.count(S) > 0 ? S : new_productions.begin()->first;
    }

    // 更新产生式集合
    P = new_productions;
}

// 消去无用符号
void eliminate_useless()
{
    //cout << "消去无用符号" << endl;
    unordered_set<char> N0, N1;
    // 遍历产生式P
    for (auto const &[key, value] : P)
    {
        // 遍历产生式右侧的规则
        for (auto const &r : value)
        {
            bool in_T = true;
            // 检查产生式右侧的符号是否全在T中
            for (auto const &i : r)
            {
                if (T.count(i) == 0)
                {
                    in_T = false;
                    break;
                }
            }
            // 如果全在T中，将产生式左侧的非终结符加入N1
            if (in_T)
            {
                N1.insert(key);
                break;
            }
        }
    }

    // 不断扩展N1，直到N1不再变化
    while (N0 != N1)
    {
        N0 = N1;
        for (auto const &[key, value] : P)
        {
            for (auto const &r : value)
            {
                bool in_N0 = true;
                for (auto const &i : r)
                {
                    if (N0.count(i) == 0 && T.count(i) == 0)
                    {
                        in_N0 = false;
                        break;
                    }
                }
                if (in_N0)
                {
                    N1.insert(key);
                    break;
                }
            }
        }
    }
    // 更新非终结符集合N
    N = std::set<char>(N1.begin(), N1.end());

    // 创建新的产生式集合P1
    unordered_map<char, vector<vector<char>>> P1;
    for (auto const &[key, value] : P)
    {
        if (N.count(key) == 0)
            continue;
        P1[key] = {};
        for (auto const &r : value)
        {
            bool in_N = true;
            for (auto const &i : r)
            {
                // 如果产生式右侧的符号不在N和T中，将in_N置为false
                if (i != 'N' && N.count(i) == 0 && T.count(i) == 0)
                {
                    in_N = false;
                    break;
                }
            }
            if (in_N)
                P1[key].push_back(r);
        }
    }
    // 更新产生式集合P为P1
    P = P1;

    // 创建新的非终结符集合new_nonterminals
    unordered_set<char> new_nonterminals = {S};
    for (auto const &r : P[S])
        for (auto const &i : r)
            new_nonterminals.insert(i);

    // 扩展new_nonterminals，直到不再变化
    while (N0 != new_nonterminals)
    {
        N0 = new_nonterminals;
        for (auto const &A : N0)
        {
            if (N.count(A) == 0)
                continue;
            for (auto const &r : P[A])
                for (auto const &i : r)
                    new_nonterminals.insert(i);
        }
    }
    // 更新非终结符集合N和终结符集合T
    for (auto it = N.begin(); it != N.end();)
    {
        if (new_nonterminals.count(*it) == 0)
            it = N.erase(it);
        else
            ++it;
    }
    for (auto it = T.begin(); it != T.end();)
    {
        if (new_nonterminals.count(*it) == 0)
            it = T.erase(it);
        else
            ++it;
    }

    // 创建新的产生式集合P2
    unordered_map<char, vector<vector<char>>> P2;
    for (auto const &[key, value] : P)
    {
        if (N.count(key) == 0)
            continue;
        P2[key] = {};
        for (auto const &r : value)
        {
            bool in_N = true;
            for (auto const &i : r)
            {
                if (i != 'N' && N.count(i) == 0 && T.count(i) == 0)
                {
                    in_N = false;
                    break;
                }
            }
            if (in_N)
                P2[key].push_back(r);
        }
    }
    // 更新产生式集合P为P2
    P = P2;
}

int main()
{
    read_input();
    //print_grammar();
   
    eliminate_epsilon();
    eliminate_single();
    eliminate_useless();

    print_grammar();

    return 0;
}