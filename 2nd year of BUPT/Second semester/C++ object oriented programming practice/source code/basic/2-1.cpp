#include <iostream>
#include <cmath>
#include <limits>
using namespace std;

// Point类定义
class Point
{
public:
    Point(double x = 0, double y = 0);
    ~Point();
    // 静态成员函数不属于某个对象，因此没有this指针
    static double distance(const Point& p1, const Point& p2);
private:
    double x, y;
};

// Point类实现

// 构造函数
Point::Point(double x, double y) : x(x), y(y)
{
    cout << "Point构造函数被调用" << endl;
}

// 析构函数
Point::~Point()
{
    cout << "Point析构函数被调用" << endl;
}

// 计算两点之间的距离
double Point::distance(const Point& p1, const Point& p2)
{
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    return sqrt(dx * dx + dy * dy);
}

// Circle类定义
class Circle
{
public:
    Circle(const Point& center, double radius);
    ~Circle();
    bool intersects(const Circle& other) const;
private:
    Point center;
    double radius;
};

// Circle类实现

// 构造函数
Circle::Circle(const Point& center, double radius) : center(center), radius(radius)
{
    cout << "Circle构造函数被调用" << endl;
}

// 析构函数
Circle::~Circle()
{
    cout << "Circle析构函数被调用" << endl;
}

// 判断两个圆是否相交
bool Circle::intersects(const Circle& other) const
{
    return Point::distance(center, other.center) <= radius + other.radius;
}

int main()
{
    double x1, y1, r1, x2, y2, r2;

    cout << "输入第一个圆的圆心坐标和半径：" << endl;
    if (!(cin >> x1 >> y1 >> r1)) { // 输入无效
        cerr << "错误：输入不是数字" << endl;
        return 1;
    }

    cout << "输入第二个圆的圆心坐标和半径：" << endl;
    if (!(cin >> x2 >> y2 >> r2)) { // 输入无效
        cerr << "错误：输入不是数字" << endl;
        return 1;
    }

    // 检查半径是否为非负数
    if (r1 < 0 || r2 < 0) {
        cerr << "错误：半径不能为负数" << endl;
        return 1;
    }

    Point center1(x1, y1);
    Point center2(x2, y2);
    Circle c1(center1, r1);
    Circle c2(center2, r2);

    if (c1.intersects(c2))
    {
        cout << "两个圆相交" << endl;
    }
    else
    {
        cout << "两个圆不相交" << endl;
    }

    return 0;
}
/*
相交
1 1 1 
2 2 1
相离
1 1 1 
10 10 0.5
*/
/*--------------------------------------------------------------------------------*/
/*
1. 两次Point构造函数被调用: 
分别对应于创建center1和center2两个Point对象的构造。
这两个对象表示输入的两个圆的圆心。
2. 两次Circle构造函数被调用: 
分别对应于创建circle1和circle2两个Circle对象的构造。
这两个对象表示输入的两个圆。

接下来是析构函数的调用。
析构函数的调用顺序与构造函数相反，这是因为对象的生命周期是从创建到销毁，
因此先创建的对象后销毁，后创建的对象先销毁。

1. 第一个Circle析构函数被调用: 对应于销毁circle2对象。
2. 第一个Point析构函数被调用: 对应于销毁circle2内部的center2对象。
3. 第二个Circle析构函数被调用: 对应于销毁circle1对象。
4. 第二个Point析构函数被调用: 对应于销毁circle1内部的center1对象。
5. 最后两次Point析构函数被调用: 分别对应于销毁center1和center2两个Point对象。

综上所述，对象的创建顺序是：center1、center2、circle1、circle2；销毁顺序正好相反。
*/