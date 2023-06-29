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
#include "schedule.h"
#include "activityschedule.h"
#include "temporaryschedule.h"
#include "CLOCK_TIME.h"
#include "log.h"


using namespace std;

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLineEdit;
class QTextEdit;

class ScheduleWindow : public QWidget
{
    Q_OBJECT

public:
    int weekNum=1;   // 当前是第几周

    void addCourse(Schedule& schedule, ActivitySchedule& activity_schedule, TemporarySchedule& temp_schedule,
                   int startWeek, int endWeek, int day, int startPeriod, int endPeriod, string course, string teacherName, string classroom,
                   int examTime, int examDay, int examStartPeriod, int examEndPeriod, string examRoom);
    ScheduleWindow(logTXT*logtxt,string fileName, QWidget *parent = nullptr);
    ~ScheduleWindow();

   // CLOCK_TIME*clk_time;
    logTXT *logtxt;
    Schedule *schedule;
    //ActivitySchedule *activity_schedule;
    //TemporarySchedule *temp_schedule;

private slots:

    void onFindCourseClicked();
    void onAddCourseClicked();
    void onCourseCellClicked(int row, int col);

private:
    QLabel *weekLabel; // 当前是第几周

    void createCourseTable();
    void updateCourseTable();

    QVBoxLayout *mainLayout;
    QHBoxLayout *searchLayout;
    QHBoxLayout *addLayout;

    QPushButton *btnShowSchedule;
    QPushButton *btnFindCourse;
    QPushButton *btnAddCourse;

    QLineEdit *searchInput;
    QTableWidget *scheduleDisplay;

    QLineEdit *addCourseInput;

    QTextEdit *findResultDisplay;



};
