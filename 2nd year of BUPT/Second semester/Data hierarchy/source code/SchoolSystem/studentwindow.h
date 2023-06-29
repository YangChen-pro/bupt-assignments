#ifndef STUDENTWINDOW_H
#define STUDENTWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFile>
#include <QTextStream>
#include <QTimer>
//#include "CLOCK_TIME.h"
#include <QTableWidget>
#include <QScrollArea>
#include "log.h"
#include "schedulewindow.h"
#include "activitywindow.h"
#include "tempwindow.h"
#include "alarmwindow.h"
#include "navigate.h"

class StudentWindow : public QWidget
{
    Q_OBJECT

public:
    explicit StudentWindow(logTXT*logtxt,const QString &username,QWidget *parent = nullptr);
    ~StudentWindow();
    logTXT *logtxt;
    int today;

private:
    QPushButton* pause;
    QLineEdit* labelTime;
    QLabel* myLabel;//显示操作信息。

    QLabel *studentLabel; // 添加 studentLabel，用于显示“学生”两个字
    QLabel *nameNumberLabel; // 添加的 QLabel，用于显示学生的姓名和学号
    QListWidget *menuListWidget;
    QStackedWidget *stackedWidget;
    QHBoxLayout *mainLayout;

    QScrollArea *scrollAreaAlarm;
    QTableWidget *tableWidgetAlarm;

};

#endif // STUDENTWINDOW_H
