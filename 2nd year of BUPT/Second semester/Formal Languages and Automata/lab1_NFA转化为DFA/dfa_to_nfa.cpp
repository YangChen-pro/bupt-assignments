#include <bits/stdc++.h>
using namespace std;
const int MAX = 99;
int cnt = 0; // 记录状态数目

vector<vector<vector<int>>> Q1; // 状态转移函数
set<int> isfinal1;              // 终止状态集合

typedef struct
{
    int from;  // 当前状态
    int to[2]; // 0转移后的状态 1转移后的状态
} shift;       // 状态转移信息

vector<shift> Q2; // DFA状态转移函数

typedef struct
{
    vector<int> State;
    int number;
} Stateset;

queue<int> Queue;           // 转换时用到的队列，存储状态在状态表StateList的下标
vector<Stateset> StateList; // DFA的状态表，每个元素代表一个DFA中的一个状态  number是状态编号，一维向量State存该状态对应代表的NFA里的状态
set<int> isfinal2;          // DFA的终态表

void input() // 输入NFA
{
    string s; // 保存每一行的字符串
    vector<vector<int>> temp1;
    vector<int> temp2;
    getline(cin, s); // 读入第一行
    while (getline(cin, s))
    {
        temp1.clear();
        temp2.clear();
        if (s.find('e') != string::npos)
            isfinal1.insert(cnt);
        int loc = s.find(']') + 1; // 从哪里开始寻找状态
        while (loc < s.size())
        {
            int num = 0;       // 状态序号
            if (s[loc] == ']') // 读完一种情况
            {
                temp1.push_back(temp2);
                temp2.clear();
            }
            else if (isdigit(s[loc])) // 读到序号
            {
                for (loc; isdigit(s[loc]); loc++)
                    num = num * 10 + (int)(s[loc] - '0');
                temp2.push_back(num);
                loc--; // *回退一位
            }
            else if (s[loc] == 'N') // 读到空
            {
                temp2.push_back(-1);
                temp1.push_back(temp2);
                temp2.clear();
            }
            loc++;
        }
        Q1.push_back(temp1);
        cnt++;
    }
}

void output(vector<shift> Q2) // 输出DFA
{
    cout << "\t\t0\t1" << endl;
    cout << "(s)q0\tq" << Q2[0].to[0] << "\tq" << Q2[0].to[1] << endl;
    for (int i = 1; i < Q2.size(); i++)
    {
        if (isfinal2.count(Q2[i].from)) // 判断是否是终止状态
            cout << "(e)";
        cout << "q" << Q2[i].from << "\t";
        if (Q2[i].to[0] == -1) // 判断是否是空
            cout << "N"
                 << "\t";
        else
            cout << "q" << Q2[i].to[0] << "\t";
        if (Q2[i].to[1] == -1) // 判断是否是空
            cout << "N" << endl;
        else
            cout << "q" << Q2[i].to[1] << endl;
    }
}

void transfer() // 转换
{
    int newnumber = Q1.size(); // 新序号

    Stateset temp1;
    temp1.State.push_back(0);
    temp1.number = 0;
    StateList.push_back(temp1);
    Queue.push(StateList.size() - 1); // 构造DFA的初始状态q0 入DFA状态表和队列

    while (Queue.size() != 0) // 当队列非空
    {
        temp1 = StateList[Queue.front()];   // 取队头元素temp1;
        shift temp3 = {temp1.number, 0, 0}; // 构造其相应转移表temp3

        // 对0转移和1转移，对temp1中包含的每一个NFA中的状态进行转换，得到转移后的状态集
        for (int j = 0; j <= 1; j++) // j=0,0转移； j=1,1转移；
        {
            vector<int> State;
            State.clear();                               // 存储j转换后得到状态集的临时变量
            for (int i = 0; i < temp1.State.size(); i++) // 针对DFA中状态temp1包含的各个NFA中的状态
            {
                if (Q1[temp1.State[i]][j][0] != -1) // 如果temp1包含的状态非空
                {
                    // 不重复地加入转换出的NFA中的状态
                    for (int k = 0; k < Q1[temp1.State[i]][j].size(); k++)
                    {
                        if (!count(State.begin(), State.end(), Q1[temp1.State[i]][j][k]))
                            State.push_back(Q1[temp1.State[i]][j][k]);
                    }
                }
            }

            if (!State.size())
                temp3.to[j] = -1; // 如果转换后状态集为空，用-1代表
            else                  // 若非空
            {
                sort(State.begin(), State.end()); // 状态集排序，便于之后的比较
                // 检测该状态集是否是新出现的情况
                int flag = -1;
                for (int k = 0; k < StateList.size(); k++)
                {
                    if (State == StateList[k].State)
                    {
                        flag = k;
                        break;
                    }
                }
                Stateset temp2; // 存储生成状态的临时变量
                if (flag == -1) // 若是新状态
                {

                    temp2.State = State;
                    if (State.size() == 1)
                        temp2.number = State[0];
                    else
                        temp2.number = newnumber++; // 给新状态赋编号（若是含多个NFA中的状态，使用新编号，若否，使用对应NFA编号） 和 对应的NFA状态集

                    // 检查新状态是否为终结状态 若是 入isfinal2集合
                    for (int j = 0; j < temp2.State.size(); j++)
                    {
                        if (isfinal1.count(temp2.State[j]) != 0)
                        {
                            isfinal2.insert(temp2.number);
                        }
                    }

                    // 将新状态入DFA状态表和工作队列
                    StateList.push_back(temp2);
                    Queue.push(StateList.size() - 1);
                }
                else
                    temp2.number = StateList[flag].number; // 若不是新状态， 则编号为对应已有状态编号

                temp3.to[j] = temp2.number; // 存入发生j转换的编号
            }
        }
        Q2.push_back(temp3); // 将本条转移信息放入Q2
        Queue.pop();         // 删除队头元素
    }
}

int compare(shift a, shift b)
{
    return a.from < b.from;
} // 排序时用到的比较函数

int main()
{
    input();
    transfer();
    sort(Q2.begin(), Q2.end(), compare); // 转移函数条目按照状态编号排序
    output(Q2);
    system("pause");
    return 0;
}
