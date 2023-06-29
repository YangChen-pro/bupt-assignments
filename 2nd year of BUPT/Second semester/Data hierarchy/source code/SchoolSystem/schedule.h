#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include <QString>
#include <cstring>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <QDebug>
#include <QFile>
#include "mymap.h"
#include "activityschedule.h"
#include "temporaryschedule.h"

using namespace std;

class Schedule
{
friend class ScheduleWindow;
friend class TemporarySchedule;
friend class ActivitySchedule;
friend class AlarmWindow;

private:

    string teacherName;        // 教师姓名
    string classroom;          // 教室
    string exam[20][7][13];    // 20周，7天，13个时间段的考试安排
    string examRoom;           // 考试教室
    fstream file;              // 文件输入输出流
    // 哈希表，用于存储课程信息
    unordered_map<string, vector<string>> courseMap; // 课程名字到课程信息的映射

    ActivitySchedule *activity_schedule;
    TemporarySchedule *temp_schedule;

public:
    string courses[16][7][13]; // 16周，7天，13节课; // 16周，7天，13节课
    Schedule(string filename); // 构造函数，filename为文件名
    ~Schedule();               // 析构函数
    string getcourses(int week, int day, int period); // 获取课程
    string getexams(int week, int day, int period); // 获取考试信息

    // 查找课程，course为课程名字
    QString FindCourse(const string &course);
    // 添加课程
    void AddCourse(int startWeek, int endWeek, int day, int startPeriod, int endPeriod, string course, string teacherName, string classroom, int examTime, int examDay, int examStartPeriod, int examEndPeriod, string examRoom);
    // 删除课程，startWeek为开始周数，endWeek为结束周数，day为星期几，startPeriod为开始节数，endPeriod为结束节数
    void DeleteCourse(int startWeek, int endWeek, int day, int startPeriod, int endPeriod);

    // 检查课程是不是和其他课程冲突
    bool checkCourseConflict(int start_week, int end_week, int day, int start_period, int end_period);

    // 检查课程是不是和活动，临时事物冲突
    bool checkOtherConflict(int start_week, int end_week, int day, int start_period, int end_period);

    void setActivitySchedule(ActivitySchedule *activitySchedule);
    void setTemporarySchedule(TemporarySchedule *tempSchedule);

};
