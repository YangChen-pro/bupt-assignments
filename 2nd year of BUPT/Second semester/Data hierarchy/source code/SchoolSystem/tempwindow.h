
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
#include <QDialogButtonBox>
#include "temporaryschedule.h"
#include "schedule.h"
#include "activityschedule.h"
#include "sortedtempdialog.h"

#include <QTableWidget>
#include <QScrollArea>

#include "CLOCK_TIME.h"
#include "log.h"

using namespace std;

class TempWindow : public QWidget
{
    Q_OBJECT

public:
   int weekNum =1;   // 当前是第几周
    string PersonalFile; // 记录个人活动文件的名字
    TempWindow(logTXT*logtxt,const string personalFile, QWidget *parent = nullptr);
    ~TempWindow();

    //Schedule *schedule;
    //ActivitySchedule *activity_schedule;
    TemporarySchedule *Temp_schedule;

    logTXT *logtxt;

private slots:
    void onFindTempClicked();
    void onAddTempClicked();
    void onTempCellClicked(int row, int col);
    void sortTempByTime();



private:


    QLabel *weekLabel;

    void createTempTable();
    void updateTempTable();

    QVBoxLayout *mainLayout;
    QHBoxLayout *searchLayout;
    QHBoxLayout *addLayout;

    QPushButton *btnShowTemp;
    QPushButton *btnFindTemp;
    QPushButton *btnAddTemp;
    QPushButton *btnSortTemp;

    QLineEdit *searchInput;
    QTableWidget *TempDisplay;

    QLineEdit *addTempInput;

    QTextEdit *findResultDisplay;


};

