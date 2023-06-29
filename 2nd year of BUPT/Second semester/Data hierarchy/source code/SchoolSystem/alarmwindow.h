#ifndef ALARMWINDOW_H
#define ALARMWINDOW_H
#pragma once
#include <QWidget>
#include <QTableWidget>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <QTextEdit>

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <cmath>
#include "CLOCK_TIME.h"
#include <QDialogButtonBox>
#include <QScrollArea>
#include "schedule.h"
#include <QTableWidget>
#include "log.h"
#include "schedule.h"
#include "activityschedule.h"
#include "temporaryschedule.h"


class AlarmWindow :public QWidget
{
   Q_OBJECT
public:
    AlarmWindow(logTXT*logtxt);
    ~AlarmWindow();


    void addClicked();
    void delClicked();
    void changeClicked();
    void setAlarmlist();
    logTXT *logtxt;
    Schedule*sche;
    ActivitySchedule*sche2;
    TemporarySchedule*sche3;


private:
    //void createAlarmTable();
   // void updateAlarmTable();

    QVBoxLayout *mainLayout;
    QHBoxLayout *searchLayout;
    QHBoxLayout *addLayout;

    QTableWidget *tableWidget;
    QScrollArea *scrollArea;

    QPushButton *getAlarm;
    QPushButton *btnAddAlarm;
    QPushButton *btnDelAlarm;
    QPushButton *btnChangeAlarm;







};

#endif // ALARMWINDOW_H
