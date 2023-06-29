#include <iostream>
#include <cmath>
#include <stdexcept>
#define PI acos(-1.0)
using namespace std;

// 基类Shape定义
class Shape
{
public:
    virtual ~Shape() { cout << "Shape析构函数被调用" << endl; }
    virtual double area() const = 0; // 纯虚函数，计算面积
};

// 矩阵类Rectangle定义
class Rectangle : public Shape
{
public:
    Rectangle(double width, double height);
    ~Rectangle();
    double area() const;
protected:
    double width, height;
};

// 矩阵类Rectangle实现
Rectangle::Rectangle(double width, double height) : width(width), height(height)
{
    if (width < 0 || height < 0)
    {
        throw invalid_argument("Error: 矩形宽度或高度不能为负数！");
    }
    cout << "Rectangle构造函数被调用" << endl;
}

// 析构函数
Rectangle::~Rectangle()
{
    cout << "Rectangle析构函数被调用" << endl;
}

// 计算矩形面积
double Rectangle::area() const
{
    return width * height;
}

// 圆类Circle定义
class Circle : public Shape
{
public:
    Circle(double radius);
    ~Circle();
    double area() const;
private:
    double radius;
};

// 圆类Circle实现
Circle::Circle(double radius) : radius(radius)
{
    if (radius < 0)
    {
        throw invalid_argument("Error: 圆的半径不能为负数！");
    }
    cout << "Circle构造函数被调用" << endl;
}

// 析构函数
Circle::~Circle()
{
    cout << "Circle析构函数被调用" << endl;
}

// 计算圆面积
double Circle::area() const
{
    return PI * radius * radius;
}

// 正方形类Square定义
class Square : public Rectangle
{
public:
    Square(double width);
    ~Square();
};

// 正方形类Square实现
Square::Square(double width) : Rectangle(width, width)
{
    cout << "Square构造函数被调用" << endl;
}

// 析构函数
Square::~Square()
{
    cout << "Square析构函数被调用" << endl;
}

int main()
{
    Shape* shapes[3];
    try {
        shapes[0] = new Rectangle(3, 4);
        shapes[1] = new Circle(5);
        shapes[2] = new Square(6);
    }
    catch (const invalid_argument& e) {
        cerr << "异常信息：" << e.what() << endl;
        return 1;
    }

    for (int i = 0; i < 3; i++)
    {
        cout << "第" << i + 1 << "个形状的面积是" << shapes[i]->area() << endl;
        delete shapes[i];
    }
    return 0;
}