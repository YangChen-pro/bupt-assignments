#include <cstdio>
#include <cstring>
#include <vector>
#include <functional>
#include <algorithm>
using namespace std;
// 从文件中读取数据
vector<int> input(const char fileName[])
{
    FILE *file = freopen(fileName, "r", stdin);
    vector<int> result;
    if (file)
    {
        int line;
        while (scanf("%d", &line) != EOF)
        {
            result.push_back(line);
        }
    }
    fclose(stdin);
    return result;
}

// 借助最长公共子序列求最长不上升子序列
void LDS_by_LCS(vector<int> num)
{
    int n = num.size();
    vector<int> num_temp = num;
    sort(num.begin(), num.end(), greater<int>());
    vector<vector<int>> dp(n + 1, vector<int>(n + 1, 0));
    vector<vector<int>> path(n + 1, vector<int>(n + 1, 0));
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; ++j)
        {
            if (num[i - 1] == num_temp[j - 1])
            {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                path[i][j] = 1;
            }
            else if (dp[i - 1][j] >= dp[i][j - 1])
            {
                dp[i][j] = dp[i - 1][j];
                path[i][j] = 2;
            }
            else
            {
                dp[i][j] = dp[i][j - 1];
                path[i][j] = 3;
            }
        }
    }
    printf("最长不上升子序列长度为：%d\n", dp[n][n]);
    printf("最长不上升子序列为：");
    typedef function<void(int, int)> PrintFunction;
    PrintFunction output = [&](int i, int j)
    {
        if (i == 0 || j == 0)
            return;
        if (path[i][j] == 1)
        {
            output(i - 1, j - 1);
            printf("%d ", num[i - 1]);
        }
        else if (path[i][j] == 2)
            output(i - 1, j);
        else
            output(i, j - 1);
    };
    output(n, n);
    printf("\n");
}

// 最长不上升子序列
void LDS(vector<int> num)
{
    int n = num.size();
    vector<int> dp(n, 1);  // dp[i]表示以num[i]结尾的最长不上升子序列的长度
    vector<int> path(n, -1);
    for (int i = 1; i < n; i++)
    {
        for (int j = 0; j <= i - 1; j++)
        {
            if (num[i] <= num[j] && dp[i] < dp[j] + 1)
            {
                dp[i] = dp[j] + 1;
                path[i] = j;
            }
        }
    }
    int ans = dp[0];
    int index = 0;
    for (int i = 0; i < n; i++)
    {
        if (ans < dp[i])
        {
            ans = dp[i];
            index = i;
        }
    }
    printf("最长不上升子序列长度为：%d\n", ans);
    printf("最长不上升子序列为：");
    typedef function<void(int)> PrintFunction;
    PrintFunction output = [&](int x)
    {
        if (x == -1)
            return;
        output(path[x]);
        printf("%d ", num[x]);
    };
    output(index);
    printf("\n");
}

// 只计算长度,时间复杂度O(nlogn)
void LDS_nlogn(vector<int> num)
{
    int n = num.size();
    // dp[i]表示长度为i的最长不上升子序列的最大末尾元素
    vector<int> dp(n + 1, INT_MAX);
    int len = 0;
    for (int i = 0; i < n; i++)
    {
        // 如果num[i]≤最长不上升子序列的最大末尾元素，那么num[i]可以接在最长不上升子序列后面，形成一个更长的不上升子序列
        if (num[i] <= dp[len]) 
        {
            dp[++len] = num[i];
        }
        else // 否则，找到dp中第一个小于num[i]的元素，用num[i]替换它
        {
            int pos = upper_bound(dp.begin() + 1, dp.begin() + len + 1, num[i], greater<int>()) - dp.begin();
            // printf("dp[pos-1] = %d, dp[pos] = %d, dp[pos+1] = %d\n", dp[pos - 1], dp[pos], dp[pos + 1]);
            // printf("num[i] = %d\n", num[i]);
            dp[pos] = num[i];
        }
    }
    printf("最长不上升子序列长度为：%d\n", len);
}

int main()
{
    vector<int> num1 = input("input2_1.txt");
    vector<int> num2 = input("input2_2.txt");
    printf("num1:\n");
    LDS_by_LCS(num1);
    LDS(num1);
    LDS_nlogn(num1);
    printf("\nnum2:\n");
    LDS_by_LCS(num2);
    LDS(num2);
    LDS_nlogn(num2);
    return 0;
}