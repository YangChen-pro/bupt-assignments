#include <iostream>
#include <iomanip>
using namespace std;

// 矩阵类定义
class Matrix
{
public:
    Matrix(int lines, int rows);
    Matrix(const Matrix& other); // 拷贝构造函数
    ~Matrix();
    Matrix& operator=(const Matrix& other); // 重载赋值运算符
    void inputMatrix();
    void printMatrix() const;
    Matrix operator+(const Matrix& other) const; // 重载加法运算符
    Matrix operator-(const Matrix& other) const; // 重载减法运算符
private:
    int **data;
    int lines, rows;
};

// 构造函数
Matrix::Matrix(int lines, int rows) : lines(lines), rows(rows)
{
    data = new int*[lines]; // 申请lines个指针
    for (int i = 0; i < lines; i++)
    {
        data[i] = new int[rows]; // 每个指针指向一个数组
    }
}

// 拷贝构造函数
Matrix::Matrix(const Matrix& other) : lines(other.lines), rows(other.rows)
{
    cout << "拷贝构造函数被调用" << endl;
    data = new int*[lines]; // 申请lines个指针
    for (int i = 0; i < lines; i++)
    {
        data[i] = new int[rows]; // 每个指针指向一个数组
        for (int j = 0; j < rows; j++)
        {
            data[i][j] = other.data[i][j];
        }
    }
}

// 析构函数
Matrix::~Matrix()
{
    for (int i = 0; i < lines; i++)
    {
        delete[] data[i]; // 释放每个数组
    }
    delete[] data; // 释放指针数组
    data = NULL; // 防止野指针
}

// 重载赋值运算符
Matrix& Matrix::operator=(const Matrix& other)
{
    cout << "赋值运算符被调用" << endl;
    if (this == &other) // 检查自赋值
    {
        return *this;
    }
    
    // 释放原有资源
    for (int i = 0; i < lines; i++)
    {
        delete[] data[i];
    }
    delete[] data;

    // 分配新资源并复制
    lines = other.lines;
    rows = other.rows;
    data = new int*[lines];
    for (int i = 0; i < lines; i++)
    {
        data[i] = new int[rows];
        for (int j = 0; j < rows; j++)
        {
            data[i][j] = other.data[i][j];
        }
    }
    return *this;
}

// 输入矩阵
void Matrix::inputMatrix()
{
    for (int i = 0; i < lines; i++)
    {
        for (int j = 0; j < rows; j++)
        {
            cin >> data[i][j];
            if (cin.fail()) { // 处理输入格式错误
                cerr << "输入数据格式错误，请重新输入！" << endl;
                cin.clear();
                cin.ignore();
                j--;
            }
        }
    }
}

// 输出矩阵
void Matrix::printMatrix() const
{
    for (int i = 0; i < lines; i++)
    {
        cout << setw(4) << data[i][0];
        for (int j = 1; j < rows; j++)
        {
            cout << " " << setw(4) << data[i][j];
        }
        cout << endl;
    }
}

// 重载加法运算符
Matrix Matrix::operator+(const Matrix& other) const
{
    if (lines != other.lines || rows != other.rows)
    {
        cerr << "矩阵大小不同，无法相加!" << endl;
        throw "Error: 矩阵大小不同，无法相加!";
    }
    Matrix result(lines, rows);
    for (int i = 0; i < lines; i++)
    {
        for (int j = 0; j < rows; j++)
        {
            result.data[i][j] = data[i][j] + other.data[i][j];
        }
    }
    return result;
}

// 重载减法运算符
Matrix Matrix::operator-(const Matrix& other) const
{
    if (lines != other.lines || rows != other.rows)
    {
        cerr << "矩阵大小不同，无法相减!" << endl;
        throw "Error: 矩阵大小不同，无法相减!";
    }
    Matrix result(lines, rows);
    for (int i = 0; i < lines; i++)
    {
        for (int j = 0; j < rows; j++)
        {
            result.data[i][j] = data[i][j] - other.data[i][j];
        }
    }
    return result;
}

int main()
{
    int lines, rows;
    cout << "请输入第一个矩阵的行数和列数：";
    cin >> lines >> rows;
    Matrix A1(lines, rows);
    cout << "请输入第一个矩阵的元素：" << endl;
    A1.inputMatrix();

    cout << "请输入第二个矩阵的行数和列数：";
    cin >> lines >> rows;
    Matrix A2(lines, rows);
    cout << "请输入第二个矩阵的元素：" << endl;
    A2.inputMatrix();

    try {
        Matrix A3 = A1 + A2;
        cout << "矩阵相加的结果为：" << endl;
        A3.printMatrix();
    } catch (const char* msg) { // 捕获异常
        cerr << "异常信息：" << msg << endl;
    }

    try {
        Matrix A3 = A1 - A2;
        cout << "矩阵相减的结果为：" << endl;
        A3.printMatrix();
    } catch (const char* msg) { // 捕获异常
        cerr << "异常信息：" << msg << endl;
    }

    Matrix* pA1 = new Matrix(lines, rows);
    cout << "请输入第一个矩阵的元素：" << endl;
    pA1->inputMatrix();

    Matrix* pA2 = new Matrix(lines, rows);
    cout << "请输入第二个矩阵的元素：" << endl;
    pA2->inputMatrix();

    try {
        Matrix* pA3 = new Matrix(lines, rows);
        *pA3 = *pA1 + *pA2;
        cout << "指针矩阵相加的结果为：" << endl;
        pA3->printMatrix();
        delete pA3;
    } catch (const char* msg) { // 捕获异常
        cerr << "异常信息：" << msg << endl;
        delete pA1;
        delete pA2;
        return -1;
    }

    try {
        Matrix* pA3 = new Matrix(lines, rows);
        *pA3 = *pA1 - *pA2;
        cout << "指针矩阵相减的结果为：" << endl;
        pA3->printMatrix();
        delete pA3;
    } catch (const char* msg) { // 捕获异常
        cerr << "异常信息：" << msg << endl;
        delete pA1;
        delete pA2;
        return -1;
    }

    delete pA1;
    delete pA2;

    return 0;
}
/*
3 3
0 0 1
0 1 2
0 2 3
*/
/*
3 3
9 6 6
2 3 3
4 1 8
*/