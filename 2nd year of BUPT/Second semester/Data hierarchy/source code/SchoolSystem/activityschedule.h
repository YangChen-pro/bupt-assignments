
#ifndef ACTIVITYSCHEDULE_H
#define ACTIVITYSCHEDULE_H

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <QDebug>
#include "activity.h"
#include "temporaryschedule.h"
#include "schedule.h"
#include "person.h"
#include "mymap.h"

#define inf 0x7fffffff

using namespace std;

class ActivitySchedule
{
friend class Schedule;
friend class TemporarySchedule;
friend class ActivityWindow;
friend class AlarmWindow;

private:
    string team_activity[20][7][16]; // 集体活动时间表
    string personal_activity[20][7][16]; // 个人活动时间表
    vector<Activity> activities;                  // 存储所有活动
    fstream file;                                 // 文件输入输出流
    unordered_map<string, vector<string>> ActivityMap; // 活动名字到活动信息的映射

    Schedule *schedule;
    TemporarySchedule *temp_schedule;

public:
    void read_activities(string filename, int type);
    ActivitySchedule(string personal_filename, string group_filename);
    ~ActivitySchedule();
    // 检查时间冲突的辅助函数
    bool checkActivityConflict(int type, int start_week, int end_week, int day, int start_period, int end_period);

    void add_activity(Activity activity, string filename); // 添加活动
    string getActivity(int week, int day, int period);
    QString find_activity(const string &name); // 查找活动
    void delete_activity(string name, string personal_filename, int startWeek, int endWeek, int day, int startPeriod, int endPeriod); // 删除活动
    int checkType(const string name); // 查看活动类型

    void setSchedule(Schedule *sched);
    void setTemporarySchedule(TemporarySchedule *tempSchedule);

};


#endif // ACTIVITYSCHEDULE_H
