#include <iostream>
#include <cmath>
#include <stdexcept>
using namespace std;

// Point类定义
class Point
{
    friend ostream &operator<<(ostream &cout, const Point &p);

public:
    Point(double x = 0, double y = 0);
    ~Point();
    Point &operator++();   // 前置++
    Point &operator--();   // 前置--
    Point operator++(int); // 后置++
    Point operator--(int); // 后置--
private:
    double x, y;
};

// Point类实现

// 构造函数
Point::Point(double x, double y) : x(x), y(y)
{
    if (isnan(x) || isnan(y) || isinf(x) || isinf(y))
    {
        throw domain_error("Error: x或y坐标不是有限数！");
    }
}

// 析构函数
Point::~Point()
{
}

// 前置++
Point &Point::operator++()
{
    ++x, ++y;
    return *this;
}

// 前置--
Point &Point::operator--()
{
    --x, --y;
    return *this;
}

// 后置++
Point Point::operator++(int)
{
    Point old = *this;
    ++(*this); // 调用前置++
    return old;
}

// 后置--
Point Point::operator--(int)
{
    Point old = *this;
    --(*this); // 调用前置--
    return old;
}

// 友元函数
ostream &operator<<(ostream &cout, const Point &p)
{
    cout << " p = (" << p.x << ", " << p.y << ")";
    return cout;
}

int main()
{
    try {
        Point p(1, 1);
        cout << (++p) << endl; // (2, 2)

        cout << (--p) << endl; // (1, 1)

        cout << (p++) << endl; // (1, 1)

        cout << p << endl; // (2, 2)

        cout << (p--) << endl; // (2, 2)

        cout << p << endl; // (1, 1)
    }
    catch (const domain_error& e) {
        cerr << "异常信息：" << e.what() << endl;
        return 1;
    }

    return 0;
}