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
#include <QDialogButtonBox>
#include "activityschedule.h"
#include "schedule.h"
#include "temporaryschedule.h"
#include "sortresultdialog.h"
#include "CLOCK_TIME.h"
#include "log.h"

using namespace std;

class ActivityWindow : public QWidget
{
    Q_OBJECT

public:
    int weekNum = 1;   // 当前是第几周
    string PersonalFile; // 记录个人活动文件的名字
    ActivityWindow(logTXT*logtxt,const string personalFile,QWidget *parent = nullptr);
    ~ActivityWindow();

    //Schedule *schedule;
    ActivitySchedule *activity_schedule;
    logTXT *logtxt;
    //TemporarySchedule *temp_schedule;
private slots:
    void onFindActivityClicked();
    void onAddActivityClicked();
    void onActivityCellClicked(int row, int col);
    void onSortActivitiesClicked();


private:
    QLabel *weekLabel;

    void createActivityTable();
    void updateActivityTable();


    QVBoxLayout *mainLayout;
    QHBoxLayout *searchLayout;
    QHBoxLayout *addLayout;

    QPushButton *btnShowActivity;
    QPushButton *btnFindActivity;
    QPushButton *btnAddActivity;
    QPushButton *btnSortActivities;


    QLineEdit *searchInput;
    QTableWidget *activityDisplay;

    QLineEdit *addActivityInput;

    QTextEdit *findResultDisplay;


};
