#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <QMainWindow>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>

namespace Ui
{
class Navigation;
}

struct my_line
{
    QLine line;
    int mode;       // 交通方式
    double cgst;    // 拥塞度
};
class Navigation : public QMainWindow
{
    Q_OBJECT

public:
    explicit Navigation(QWidget* parent = nullptr);
    ~Navigation();

    // 绘制导航
    void DrawLine(QLine line, double time, int mode, double congestion);
    // 显示拥塞度
    void show_congest(double congest);
    // 清空画线
    void delete_lines();

private:
    Ui::Navigation* ui;
    QTimer* timer;
    // 绘制导航
    void paintEvent(QPaintEvent*);
    // 获取地图建筑物的坐标
    void mousePressEvent(QMouseEvent* e);
    // 步行、骑车标志的位置
    int px1, py1, px2, py2;
    // 交通工具
    QPixmap transportation;
};

#endif // NAVIGATION_H
