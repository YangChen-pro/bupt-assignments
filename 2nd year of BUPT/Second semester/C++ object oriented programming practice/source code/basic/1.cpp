#include <iostream>
#include <iomanip>
#include <limits> // for numeric_limits
using namespace std;

// 矩阵类定义
class Matrix
{
public:
    Matrix(int rows, int cols);
    ~Matrix();
    void inputMatrix();
    void printMatrix() const;
    bool addMatrix(const Matrix &m1, const Matrix &m2);
    bool subMatrix(const Matrix &m1, const Matrix &m2);
private:
    int **data;
    int rows, cols;
};

// 构造函数
Matrix::Matrix(int rows, int cols) : rows(rows), cols(cols)
{
    data = new int*[rows]; // 申请rows个指针
    for (int i = 0; i < rows; i++)
    {
        data[i] = new int[cols]; // 每个指针指向一个数组
    }
}

// 析构函数
Matrix::~Matrix()
{
    for (int i = 0; i < rows; i++)
    {
        delete[] data[i]; // 释放每个数组
    }
    delete[] data; // 释放指针数组
}

// 输入矩阵
void Matrix::inputMatrix()
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (!(cin >> data[i][j])) { // 输入无效
                cerr << "错误：输入不是整数" << endl;
                cin.clear(); // 清空输入流
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 跳过当前行
                return;
            }
        }
    }
}

// 输出矩阵
void Matrix::printMatrix() const
{
    for (int i = 0; i < rows; i++)
    {
        cout << setw(4) << data[i][0];
        for (int j = 1; j < cols; j++)
        {
            cout << " " << setw(4) << data[i][j];
        }
        cout << endl;
    }
}

// 矩阵相加
bool Matrix::addMatrix(const Matrix &m1, const Matrix &m2)
{
    if (m1.rows != m2.rows || m1.cols != m2.cols)
    {
        cout << "矩阵大小不同，无法相加!" << endl;
        return false;
    }
    rows = m1.rows, cols = m1.cols;
    for (int i = 0; i < m1.rows; i++)
    {
        for (int j = 0; j < m1.cols; j++)
        {
            data[i][j] = m1.data[i][j] + m2.data[i][j];
        }
    }
    return true;
}

// 矩阵相减
bool Matrix::subMatrix(const Matrix &m1, const Matrix &m2)
{
    if (m1.rows != m2.rows || m1.cols != m2.cols)
    {
        cout << "矩阵大小不同，无法相减!" << endl;
        return false;
    }
    rows = m1.rows, cols = m1.cols;
    for (int i = 0; i < m1.rows; i++)
    {
        for (int j = 0; j < m1.cols; j++)
        {
            data[i][j] = m1.data[i][j] - m2.data[i][j];
        }
    }
    return true;
}

int main() 
{
    Matrix A1(4, 5), A2(4, 5), A3(4, 5);
    cout << "输出矩阵A1:" << endl;
    A1.inputMatrix();
    cout << "输出矩阵A2:" << endl;
    A2.inputMatrix();

    if (A3.addMatrix(A1, A2))
    {
        cout << "输出矩阵A3=A1+A2:" << endl;
        A3.printMatrix();
    }

    if (A3.subMatrix(A1, A2))
    {
        cout << "输出矩阵A3=A1-A2:" << endl;
        A3.printMatrix();
    }

    return 0;
}
/*
1 2 3 4 5
6 7 8 9 8
7 6 5 4 3
2 1 2 3 4
*/
/*
9 8 7 6 5
4 3 2 1 2
3 4 5 6 7
8 9 8 7 6
*/