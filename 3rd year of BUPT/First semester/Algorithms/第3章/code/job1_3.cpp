#include <cstdio>
#include <iostream>
#include <vector>

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

// 最大子段和, 分治法, O(nlogn)
int MaxSubSum(vector<int> num, int left, int right)
{
    if (left == right)
    {
        return num[left] > 0 ? num[left] : 0;
    }
    int mid = (left + right) / 2;
    int leftMax = MaxSubSum(num, left, mid);
    int rightMax = MaxSubSum(num, mid + 1, right);
    int leftBorderMax = num[mid];
    int leftBorderSum = 0;
    for (int i = mid; i >= left; i--)
    {
        leftBorderSum += num[i];
        if (leftBorderSum > leftBorderMax)
        {
            leftBorderMax = leftBorderSum;
        }
    }
    int rightBorderMax = num[mid + 1];
    int rightBorderSum = 0;
    for (int i = mid + 1; i <= right; i++)
    {
        rightBorderSum += num[i];
        if (rightBorderSum > rightBorderMax)
        {
            rightBorderMax = rightBorderSum;
        }
    }
    int borderMax = leftBorderMax + rightBorderMax;
    return max(max(leftMax, rightMax), borderMax);
}
// 最大子段和, 分治法, 重载隐藏参数
int MaxSubSum(vector<int> num)
{
    return MaxSubSum(num, 0, num.size() - 1);
}

// 最大子序列和, 动态规划, O(n)
void MaxSum(vector<int> num)
{
    int n = num.size();
    int dp = num[0], ans = num[0];
    int left = 0, right = 0;
    for (int i = 1; i < n; i++)
    {
        if (dp > 0)
        {
            dp += num[i];
        }
        else
        {
            dp = num[i];
            left = i;
        }
        if (dp > ans)
        {
            ans = dp;
            right = i;
        }
    }
    printf("最大子序列和为：%d\n", ans);
    printf("最大子序列为：");
    for (int i = left; i <= right; i++)
    {
        printf("%d ", num[i]);
    }
    printf("\n");
}

int main()
{
    vector<int> num1 = input("input2_1.txt");
    vector<int> num2 = input("input2_2.txt");
    
    printf("num1最大子段和为: %d\n", MaxSubSum(num1));
    printf("num1");
    MaxSum(num1);

    printf("\nnum2最大子段和为: %d\n", MaxSubSum(num2));
    printf("num2");
    MaxSum(num2);
    return 0;
}