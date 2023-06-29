#include "temporaryschedule.h"
#include "schedule.h"

extern MyMap<int, string> int_to_day;
extern MyMap<string, int> day_to_int;

TemporarySchedule::TemporarySchedule(string filename)
{
    file.open(filename, ios::in);
    if (!file.is_open())
    {
        cout << "打开文件失败: " << filename << endl;
            return;
    }
    string line;
    while (getline(file, line))
    {
        vector<string> tokens;
        size_t pos = 0;
        while ((pos = line.find(' ')) != string::npos) // 以空格分割字符串
        {
            tokens.push_back(line.substr(0, pos));
            line.erase(0, pos + 1);
        }
        tokens.push_back(line);
        if (tokens.size() != 6)
        {
            continue;
        }
        //qDebug() << tokens[1] << tokens[3] << tokens[4];
        string name = tokens[0];
        int week = stoi(tokens[1]);
        int day = day_to_int[tokens[2]];
        int hour = stoi(tokens[3]);
        int minute = stoi(tokens[4]);
        string location = tokens[5];

        if (temp_things[week-1][day][hour-1] != "")
            temp_things[week-1][day][hour-1] += "\n" + name;
        else
            temp_things[week-1][day][hour-1] = name;
        //qDebug()<< week-1;
        //qDebug()<< day;
        //qDebug() << hour;
        qDebug() << "temp正常";
        TemporaryThing thing(name, week, day, hour, minute, location);
        things.push_back(thing);
        TempMap[name].push_back("第" + tokens[1] + "周 " + tokens[2] + " " + to_string(hour + 5) + "点" + tokens[4] + "分 " + name);
        TempMap[name].push_back("地点：" + location);
    }
    file.close();
}

TemporarySchedule::~TemporarySchedule(){}

void TemporarySchedule::setSchedule(Schedule *sched) {
    schedule = sched;
}

void TemporarySchedule::setActivitySchedule(ActivitySchedule *activitySchedule) {
    activity_schedule = activitySchedule;
}

string TemporarySchedule::getTemp(int week, int day, int hour)
{
    //qDebug() << week-1 << day << hour - 1;
    return temp_things[week-1][day][hour-1];
}

bool TemporarySchedule::checkTempConflict(int week, int day, int hour)
{
    qDebug() << week << day << hour;
    if (activity_schedule->personal_activity[week-1][day-1][hour-1] != "" || activity_schedule->team_activity[week-1][day-1][hour-1] != "")
    {
        qDebug() << "和活动冲突";
        qDebug() << QString::fromStdString(activity_schedule->personal_activity[week-1][day-1][hour-1]);
        qDebug() << QString::fromStdString(activity_schedule->team_activity[week-1][day-1][hour-1]);
        return true;
    }
    if (hour >= 3 && hour <= 15)
    {
        if (schedule->courses[week-1][day-1][hour-3] != "")
        {
            qDebug() << "和课程冲突";
            qDebug() << QString::fromStdString(schedule->courses[week-1][day-1][hour-3]);
            return true;
        }
    }
    return false;
}


// 添加临时事务
void TemporarySchedule::add_thing(TemporaryThing thing, string filename)
{
    things.push_back(thing);
    file.open(filename, ios::app);
    if (!file.is_open())
    {
        qDebug() << "打开临时事物文件失败";
        return;
    }

//    qDebug() << thing.week << thing.day << thing.hour;
    file << thing.name << " " << thing.week << " 周" << int_to_day[thing.day] << " " << thing.hour << " " << thing.minute << " " << thing.location << endl;
    file.close();
    if (temp_things[thing.week-1][thing.day][thing.hour-1] != "") // 保证同一时间段内可以显示多个临时事务
        temp_things[thing.week-1][thing.day][thing.hour-1] += "\n" + thing.name;
    else
        temp_things[thing.week-1][thing.day][thing.hour-1] = thing.name;
    TempMap[thing.name].push_back("第" + to_string(thing.week) + "周 周" + int_to_day[thing.day] + " " + to_string(thing.hour + 5) + "点" + to_string(thing.minute) + "分 " + thing.name);
    TempMap[thing.name].push_back("地点：" + thing.location);
}

// 查找临时事务
QString TemporarySchedule::find_thing(const string &name)
{
    QString result;
    if (TempMap.count(name) == 0)
    {
        result = "查找失败，未找到临时事物";
    }
    else
    {
        result = QString::fromStdString(name) + "的活动信息如下：\n";
        for (const auto &info : TempMap[name]) // 遍历哈希表
        {
            result += QString::fromStdString(info) + "\n";
        }
    }
    return result;
}

// 删除临时事务
void TemporarySchedule::delete_thing(string name, string filename)
{
    auto it = find_if(things.begin(), things.end(), [&](const TemporaryThing &thing)
                      { return thing.name == name; });
    if (it == things.end())
    {
        qDebug() << "未找到临时事务：" << QString::fromStdString(name);
        return;
    }
    temp_things[it->week - 1][it->day][it->hour - 1] = "";

    //qDebug() << it->week << it->day << it->hour;
    things.erase(it);
    TempMap.erase(name);

    // 更新文件
    ofstream file(filename);
    if (!file.is_open())
    {
        qDebug() << "打开个人临时事务文件失败";
        return;
    }
    for (const auto &thing : things)
    {
        file << thing.name << " " << thing.week << " 周" << int_to_day[thing.day] << " " << thing.hour << " " << thing.minute << " " << thing.location << endl;
        if (thing.name == name) // 同名不同时
        {
            TempMap[thing.name].push_back("第" + to_string(thing.week) + "周 周" + int_to_day[thing.day] + " " + to_string(thing.hour + 5) + "点" + to_string(thing.minute) + "分 " + thing.name);
            TempMap[thing.name].push_back("地点：" + thing.location);
        }
    }
    file.close();
}

bool TemporarySchedule::checkTempScheduleConflict(int week, int day, int hour){
    if (temp_things[week-1][day-1][hour-1] != "")
        return true;
    return false;
}
