#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <iomanip>

using namespace std;

class TemporaryThing
{
public:
    string name;     // 事物名称
    int week;        // 第几周
    int day;         // 星期几
    int hour;        // 开始的小时数
    int minute;      // 开始的分钟数
    string location; // 事物地点

    TemporaryThing(string name, int week, int day, int hour, int minute, string location);
    bool isValid() const;
    bool isOverlapping(const TemporaryThing& other) const;

};
