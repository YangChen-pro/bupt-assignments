#include "activityschedule.h"

extern MyMap<string, int> day_to_int;
extern MyMap<int, string> int_to_day;

void ActivitySchedule::read_activities(string filename, int type)
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
        if (tokens.size() != 5)
        {
            continue;
        }
        string name = tokens[0];
        string location = tokens[4];
        int start_week = stoi(tokens[1].substr(0, tokens[1].find('-')));
        int end_week = stoi(tokens[1].substr(tokens[1].find('-') + 1));
        int day = day_to_int[tokens[2]];
        int start_period = stoi(tokens[3].substr(0, tokens[3].find('-')));
        int end_period = stoi(tokens[3].substr(tokens[3].find('-') + 1));

        qDebug() << "activity正常";
        string periodRange = to_string(start_period + 5) + "-" + to_string(end_period + 6);


//        qDebug() << filename;
//        qDebug() << start_week;
        if (type == 1)
        {
            ActivityMap[name].push_back("类别:个人活动");
            for (int i = start_week -1; i < end_week; i++)
            {
                for (int j = start_period -1 ; j < end_period; j++)
                {
                    personal_activity[i][day][j] = name;
                }
            }
        }
        else
        {
            ActivityMap[name].push_back("类别:集体活动");
            for (int i = start_week - 1; i < end_week; i++)
            {
                for (int j = start_period - 1; j < end_period; j++)
                {
                    team_activity[i][day][j] = name;
                }
            }
        }

        ActivityMap[name].push_back("第" + tokens[1] + "周 " + tokens[2] + " " + periodRange + "点 " + name);
        ActivityMap[name].push_back("地点：" + tokens[4]);

        Activity activity(name, start_week, end_week, day, start_period, end_period, location, type);
        activities.push_back(activity);
    }
    file.close();
}

ActivitySchedule::ActivitySchedule(string personal_filename, string group_filename)
{
    // 读取个人活动文件
    read_activities(personal_filename, 1);
    // 读取集体活动文件
    read_activities(group_filename, 2);
}

ActivitySchedule::~ActivitySchedule() {}

void ActivitySchedule::setSchedule(Schedule *sched) {
    schedule = sched;
}

void ActivitySchedule::setTemporarySchedule(TemporarySchedule *tempSchedule) {
    temp_schedule = tempSchedule;
}


bool ActivitySchedule::checkActivityConflict(int type, int start_week, int end_week, int day, int start_period, int end_period)
{
    qDebug() << type << start_week << end_week << day <<start_period << end_period;
    for (int i = start_week - 1; i < end_week; i++)
    {
        for (int j = start_period - 1; j < end_period; j++)
        {
            if (j >= 2 && j <= 14)
            {
                if (schedule->courses[i][day-1][j - 2] != "")
                {
                    qDebug() << "课程冲突";
                    qDebug() << i << day - 1 << j - 2;
                    qDebug() << QString::fromStdString(schedule->courses[i][day-1][j-2]);
                    return true;
                }
            }
            else
            {
                if (team_activity[i][day-1][j] != "" || personal_activity[i][day-1][j] != "")
                {
                    qDebug() << "个人的活动冲突";
                    qDebug() << i << day - 1 << j;
                    qDebug() << QString::fromStdString(team_activity[i][day-1][j]);
                    qDebug() << QString::fromStdString(personal_activity[i][day-1][j]);
                    return true;
                }
            }
        }
    }
    if (type == 1) // 个人活动不冲突
        return false;
    else
    {
        Person p;
        for (int i = start_week - 1; i < end_week; i++)
        {
            for (int j = start_period - 1; j < end_period; j++)
            {
                if (p.things[i][day-1][j] != 0)
                {
                    qDebug() << "和集体里其他人的日程安排冲突";
                    qDebug() << i << day - 1 << j;
                    qDebug() << p.things[i][day-1][j];
                    return true;
                }
            }
        }
        return false;
    }
}

void ActivitySchedule::add_activity(Activity activity, string filename)
{
    activities.push_back(activity);
    if (activity.activity_type == 1)
    {
        file.open(filename, ios::app);
        if (!file.is_open())
        {
            qDebug() << "打开个人活动文件失败";
            return;
        }
        file << activity.name << " " << activity.start_week << "-" << activity.end_week << " 周" << int_to_day[activity.day] << " " << activity.start_period << "-" << activity.end_period << " " << activity.location << endl;
        file.close();
        // 时间表同步更新
        for (int i = activity.start_week - 1; i < activity.end_week; i++)
        {
            for (int j = activity.start_period - 1; j < activity.end_period; j++)
            {
                personal_activity[i][activity.day][j] = activity.name;
            }
        }
    }
    else
    {
        file.open("group_activities.txt", ios::app);
        if (!file.is_open())
        {
            qDebug() << "打开集体活动文件失败";
            return;
        }
        file << activity.name << " " << activity.start_week << "-" << activity.end_week << " 周" << int_to_day[activity.day] << " " << activity.start_period << "-" << activity.end_period << " " << activity.location << endl;
        file.close();
        // 时间表同步更新
        for (int i = activity.start_week - 1; i < activity.end_week; i++)
        {
            for (int j = activity.start_period - 1; j < activity.end_period; j++)
            {
                team_activity[i][activity.day][j] = activity.name;
            }
        }
    }
    // 更新哈希表查找信息
    string weekRang = to_string(activity.start_week) + "-" + to_string(activity.end_week);
    string periodRange = to_string(activity.start_period + 5) + "-" + to_string(activity.end_period + 6);
    ActivityMap[activity.name].push_back("第" + weekRang + "周 周" + int_to_day[activity.day] + " " + periodRange + "点 " + activity.name);
    ActivityMap[activity.name].push_back("地点：" + activity.location);
}


QString ActivitySchedule::find_activity(const string &name)
{
    QString result;
    if (ActivityMap.count(name) == 0)
    {
        result = "查找失败，未找到活动";
    }
    else
    {
        result = QString::fromStdString(name) + "的活动信息如下：\n";
        for (const auto &info : ActivityMap[name]) // 遍历哈希表
        {
            result += QString::fromStdString(info) + "\n";
        }
    }
    return result;
}

int ActivitySchedule::checkType(const string name){
    if (ActivityMap.count(name) == 0)
    {
        return -1; // 没有这个活动
    }
    else
    {
        for (const auto &info : ActivityMap[name])
        {
            if (info == "类别:个人活动")
                return 1;
            else
                return 2;
        }
    }
}

void ActivitySchedule::delete_activity(string name, string personal_filename, int startWeek, int endWeek, int day, int startPeriod, int endPeriod)
{
    // 检查活动是否存在
    if (ActivityMap.count(name) == 0)
    {
        qDebug() << "未找到活动：";
        return;
    }

    int Type = checkType(name);
    if (Type != 1 && Type != 2)
    {
        qDebug() << "无法确定活动类型：";
        return;
    }
    // 从 activities 向量中删除活动
    auto it = find_if(activities.begin(), activities.end(), [&](const Activity &thing)
                      { return thing.name == name; });
    if (it == activities.end())
    {
        qDebug() << "未找到活动：" << QString::fromStdString(name);
        return;
    }
    activities.erase(it);

    // 从 ActivityMap 中删除活动
    ActivityMap.erase(name);


    // 清除数组的信息
    if (Type == 2)
    {
        for (int i = startWeek - 1; i < endWeek; i++)
        {
            for (int j = startPeriod - 1; j < endPeriod; j++)
                team_activity[i][day][j] = "";
        }
    }
    else
    {
        for (int i = startWeek - 1; i < endWeek; i++)
        {
            for (int j = startPeriod - 1; j < endPeriod; j++)
                personal_activity[i][day][j] = "";
        }
    }
    // 重新写入文件
    // 打开个人活动文件
    ofstream personal_file(personal_filename, ios::out);
    if (!personal_file.is_open())
    {
        qDebug() << "打开个人活动文件失败";
        return;
    }

    // 打开团体活动文件
    ofstream group_file("group_activities.txt", ios::out);
    if (!group_file.is_open())
    {
        qDebug() << "打开文件失败: group_activities.txt";
        return;
    }

    // 重新写入文件
    for (const auto &activity : activities)
    {
        Type = checkType(activity.name);
        if (Type == 1)
        {
            personal_file << activity.name << " " << activity.start_week << "-" << activity.end_week << " 周" << int_to_day[activity.day] << " " << activity.start_period << "-" << activity.end_period << " " << activity.location << endl;
        }
        else
        {
            group_file << activity.name << " " << activity.start_week << "-" << activity.end_week << " 周" << int_to_day[activity.day] << " " << activity.start_period << "-" << activity.end_period << " " << activity.location << endl;
        }
    }

    // 关闭文件
    personal_file.close();
    group_file.close();
}


string ActivitySchedule::getActivity(int week, int day, int period){
    if (team_activity[week-1][day][period] != "")
        return team_activity[week-1][day][period];
    else
        return personal_activity[week-1][day][period];
}
