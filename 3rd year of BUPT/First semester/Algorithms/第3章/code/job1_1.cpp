#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;

char *A, *B, *C, *D;
// 求最大值
int Max(int a, int b)
{
    return a > b ? a : b;
}

// 读取文件
void input()
{
    FILE *file = freopen("input1.txt", "r", stdin);
    if (file)
    {
        const int bufferSize = 1500;
        char *line = new char[bufferSize];
        char currentChar = '\0';
        while (fgets(line, bufferSize, file))
        {
            // printf("line is %s\n", line);
            if (line[0] != '\n' && line[0] != ' ')
            {
                int len = strlen(line);
                if (line[1] == ':')
                {
                    currentChar = line[0];
                }
                else
                {
                    line[len - 1] = '\0'; // 去掉换行符
                    switch (currentChar)
                    {
                    case 'A':
                        A = new char[len];
                        strcpy(A, line);
                        // printf("%s\n", A);
                        break;
                    case 'B':
                        B = new char[len];

                        strcpy(B, line);
                        // printf("%s\n", B);
                        break;
                    case 'C':
                        C = new char[len];
                        strcpy(C, line);
                        // printf("%s\n", C);
                        break;
                    case 'D':
                        D = new char[len];
                        strcpy(D, line);
                        // printf("%s\n", D);
                        break;
                    default:
                        break;
                    }
                }
            }
        }
        delete[] line;
    }
    fclose(stdin);
}

// 递归输出LCS
void LCS(int i, int j, char *X, int **path)
{
    if (i == 0 || j == 0)
        return;
    if (path[i][j] == 1)
    {
        LCS(i - 1, j - 1, X, path);
        printf("%c", X[i]);
    }
    else if (path[i][j] == 2)
        LCS(i - 1, j, X, path);
    else
        LCS(i, j - 1, X, path);
}
// PPT上的算法,空间复杂度O(mn)
void LCSLength(char *X, char *Y)
{
    int m = strlen(X);
    int n = strlen(Y);
    int **dp = new int *[m + 1];
    int **path = new int *[m + 1];
    for (int i = 0; i <= m; i++)
    {
        dp[i] = new int[n + 1];
        path[i] = new int[n + 1];
        memset(dp[i], 0, sizeof(int) * (n + 1));
        memset(path[i], 0, sizeof(int) * (n + 1));
    }
    for (int i = 1; i <= m; i++)
    {
        for (int j = 1; j <= n; j++)
        {
                if (X[i] == Y[j])
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
    LCS(m, n, X, path);
    printf("\nLCSLength is: %d\n", dp[m][n]);
    for (int i = 0; i <= m; i++)
    {
        delete[] path[i];
        delete[] dp[i];
    }
    delete[] dp;
    delete[] path;
}

// 只计算长度,空间复杂度O(n)
void LCS_Spacesaving(char *X, char *Y)
{
    int m = strlen(X);
    int n = strlen(Y);
    int *dp = new int[n + 1];
    for (int i = 0; i <= n; i++)
    {
        dp[i] = 0;
    }
    for (int i = 1; i <= m; i++)
    {
        int upLeft = dp[0]; // 保存左上角的值
        for (int j = 1; j <= n; j++)
        {
            int temp = dp[j]; // 保存当前位置的值,即dp[i-1][j],因为dp[j]已经被更新了
            if (X[i] == Y[j])
            {
                dp[j] = upLeft + 1;
            }
            else
            {
                dp[j] = Max(dp[j], dp[j - 1]);
            }
            upLeft = temp; // 更新左上角的值
        }
    }
    printf("LCSLength is: %d\n", dp[n]);
    delete[] dp;
}

int main()
{
    input();
    printf("A and B LCS:\n");
    LCSLength(A, B);
    LCS_Spacesaving(A, B);

    printf("\nA and C LCS:\n");
    LCSLength(C, D);
    LCS_Spacesaving(C, D);

    printf("\nA and D LCS:\n");
    LCSLength(A, D);
    LCS_Spacesaving(A, D);

    printf("\nC and B LCS:\n");
    LCSLength(C, B);
    LCS_Spacesaving(C, B);
    return 0;
}