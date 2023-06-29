#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <fstream>

using namespace std;

class Activity
{
public:
    string name;      // 活动名称
    int start_week;   // 开始周数
    int end_week;     // 结束周数
    int day;          // 星期几
    int start_period; // 开始节数
    int end_period;   // 结束节数
    string location;  // 活动地点
    int activity_type; // 1: 个人活动，2: 集体活动


    Activity(string name, int start_week, int end_week, int day, int start_period, int end_period, string location, int activity_type);
    string getName() const;
    int getType() const;
};
