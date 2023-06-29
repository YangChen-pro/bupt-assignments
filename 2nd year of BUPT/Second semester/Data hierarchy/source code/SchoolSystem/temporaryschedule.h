#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <iomanip>
#include <QString>
#include <unordered_map>
#include <QDebug>
#include "mymap.h"
#include "temporarything.h"

class Schedule;
class ActivitySchedule;
using namespace std;

class TemporarySchedule
{
friend class Schedule;
friend class ActivitySchedule;
friend class TempWindow;
friend class AlarmWindow;

private:
    vector<TemporaryThing> things; // 存储所有临时事务
    fstream file;                  // 文件输入输出流
    // 哈希表，用于存储临时事物信息
    unordered_map<string, vector<string>> TempMap; // 事物名字到事物信息的映射

    Schedule *schedule;
    ActivitySchedule *activity_schedule;
public:
    string temp_things[20][7][16]; // 临时事物时间表
    TemporarySchedule(string filename);
    ~TemporarySchedule();

    bool checkTempScheduleConflict(int week, int day, int hour);
    string getTemp(int week, int day, int hour);

    void add_thing(TemporaryThing thing, string filename); // 添加临时事物
    QString find_thing(const string &name); // 查找临时事务
    void delete_thing(string name, string filename); // 删除临时事务
    void sort_things();// 排序临时事务

    void setSchedule(Schedule *sched);
    void setActivitySchedule(ActivitySchedule *activitySchedule);

    bool checkTempConflict(int week, int day, int hour);
};

