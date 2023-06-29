#include "navigation.h"
#include "ui_navigation.h"

const int RefreshTimesPerSecond = 60;   // 每秒的刷新次数
QVector<my_line> paintLines;            // 已画的直线
QVector<my_line> tempLines;             // 待画的直线

void Navigation::DrawLine(QLine line, double time, int mode, double congestion)
{
    double dividedNum = time * RefreshTimesPerSecond;
    if (time == 0)
        dividedNum = 1;
    double sx = line.p1().x(); // 直线的起始点
    double sy = line.p1().y();
    double dx = line.dx() / dividedNum; // 每一小段直线的 x 方向长度
    double dy = line.dy() / dividedNum; // 每一小段直线的 y 方向长度

    for (size_t i = 0; i < dividedNum; i++)
    {
        my_line ml;
        ml.line = QLine(QPoint(sx + i * dx, sy + i * dy), QPoint(sx + (i + 1) * dx, sy + (i + 1) * dy));
        ml.mode = mode;
        ml.cgst = congestion;
        tempLines.push_back(ml); // 将每一小段直线添加到待画直线中
    }
}

Navigation::Navigation(QWidget* parent) : QMainWindow(parent),
    ui(new Ui::Navigation)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [ = ]()
    {
        if(tempLines.size())
        {
            paintLines.push_back(tempLines[0]);     //将待画直线添加进已画直线
            if(tempLines[0].mode == 0)
            {
                px1 =  paintLines.back().line.p1().x() - 6;
                py1 = paintLines.back().line.p1().y() - 6;
                px2 = -100;
                py2 = -100;
            }
            else
            {
                px2 =  paintLines.back().line.p1().x() - 6;
                py2 = paintLines.back().line.p1().y() - 6;
                px1 = -100;
                py1 = -100;
            }
            ui->congest_label->setText(QString("当前道路拥塞度为：%1").arg(tempLines[0].cgst));
            tempLines.erase(tempLines.begin());
            //qDebug() << px << " " << py;
        }
    });
    timer->start(1000 / RefreshTimesPerSecond);
}

Navigation::~Navigation()
{
    delete ui;
}

void Navigation::mousePressEvent(QMouseEvent* e)
{
    qDebug() << e->position();
}

void Navigation::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QPen pen;
    pen.setWidth(3);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPixmap(11, 7, QPixmap(":/image/graph.jpg"));

    for(int i = 0; i < paintLines.size(); i++)
    {
        // 选择交通方式
        if(paintLines[i].mode == 0)
        {
            pen.setColor(QColor(125, 125, 125));
            painter.setPen(pen);
            painter.drawPixmap(px1, py1, QPixmap(":/image/walk.png"));
        }
        else
        {
            pen.setColor(QColor(255, 0, 0));
            painter.setPen(pen);
            painter.drawPixmap(px2, py2, QPixmap(":/image/bicycle.png"));
        }
        painter.drawLine(paintLines[i].line);
    }
    update(); // 刷新界面
}

void Navigation::delete_lines()
{
    int size1=paintLines.size();
    int size2=tempLines.size();
    for(int i=0;i<size1;i++)
        paintLines.pop_back();
    for(int i=0;i<size2;i++)
        tempLines.pop_back();
    update();
}
