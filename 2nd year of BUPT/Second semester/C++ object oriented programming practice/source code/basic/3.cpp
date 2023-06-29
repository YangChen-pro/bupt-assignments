#include <iostream>
#include <cmath>
#include <stdexcept>
#define PI acos(-1.0)
using namespace std;

// 基类Shape定义
class Shape
{
public:
    virtual ~Shape() {cout << "Shape析构函数被调用" << endl;}
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
Rectangle::Rectangle(double width, double height): width(width), height(height)
{
    if (width <= 0 || height <= 0) { // 处理非法参数
        throw invalid_argument("Error: 矩形的长和宽必须为正数!");
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
    if (radius <= 0) { // 处理非法参数
        throw invalid_argument("Error: 圆的半径必须为正数!");
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

        for (int i = 0; i < 3; i++)
        {
            cout << "第" << i + 1 << "个形状的面积是" << shapes[i]->area() << endl;
            delete shapes[i];
        }
    } catch (const invalid_argument& e) { // 捕获异常
        cerr << "异常信息：" << e.what() << endl;
    }

    return 0;
}
/*
构造函数的调用顺序是从基类到派生类，
而析构函数的调用顺序是从派生类到基类。
这是因为在创建一个派生类对象时，需要先构造基类部分，然后才能构造派生类部分。
而在销毁一个派生类对象时，需要先销毁派生类部分，然后再销毁基类部分。
*/