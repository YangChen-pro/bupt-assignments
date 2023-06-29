#include "schedule.h"
#include <QDebug>
extern MyMap<string, int> day_to_int;
extern MyMap<int, string> int_to_day;
Schedule::Schedule(string filename)
{
    file.open(filename);
    if (!file)
    {
        cerr << "错误：不能打开文件 " << filename << endl;
        exit(1);
    }
    string line;
    while (getline(file, line))
    {
        stringstream ss(line);
        string courseName;      // 课程名字
        string weekRange;       // 周数范围
        string dayOfWeek;       // 星期几
        string periodRange;     // 节数范围
        string teacherName;     // 教师姓名
        string classroom;       // 教室
        string examWeek;        // 考试周
        string examDayOfWeek;   // 考试星期几
        string examPeriodRange; // 考试时间范围
        string examRoom;        // 考试教室
        getline(ss, courseName, ' ');
        getline(ss, weekRange, ' ');
        getline(ss, dayOfWeek, ' ');
        getline(ss, periodRange, ' ');
        getline(ss, teacherName, ' ');
        getline(ss, classroom, ' ');
        getline(ss, examWeek, ' ');
        getline(ss, examDayOfWeek, ' ');
        getline(ss, examPeriodRange, ' ');
        getline(ss, examRoom);
        if (courseName == "")
            break;


        int startWeek = stoi(weekRange.substr(0, weekRange.find('-')));
        int endWeek = stoi(weekRange.substr(weekRange.find('-') + 1));
        int day = day_to_int[dayOfWeek.substr(dayOfWeek.find("周"), strlen("周") + strlen("一"))];
        int startPeriod = stoi(periodRange.substr(0, periodRange.find('-')));
        int endPeriod = stoi(periodRange.substr(periodRange.find('-') + 1));

        int examTime = stoi(examWeek);
        int examDay = day_to_int[examDayOfWeek.substr(examDayOfWeek.find("周"), strlen("周") + strlen("一"))];
        int examStartPeriod = stoi(examPeriodRange.substr(0, examPeriodRange.find('-')));
        int examEndPeriod = stoi(examPeriodRange.substr(examPeriodRange.find('-') + 1));

        //qDebug() << "schedule正常";
        this->teacherName = teacherName;
        this->classroom = classroom;
        this->examRoom = examRoom;


        //qDebug() << QString::fromStdString(courseName);
        //qDebug() <<startWeek << day << startPeriod;
        for (int i = startWeek - 1; i < endWeek; i++)
        {
            for (int j = startPeriod - 1; j < endPeriod; j++)
            {
                courses[i][day][j] = courseName;
            }
        }
        courseMap[courseName].push_back("第" + weekRange + "周 " + dayOfWeek + " 第" + periodRange + "节 上课");
        courseMap[courseName].push_back("教室：" + classroom);
        courseMap[courseName].push_back("教师：" + teacherName);
        for (int i = examTime - 1; i < examTime; i++)
        {
            for (int j = examStartPeriod - 1; j < examEndPeriod; j++)
            {
                exam[i][examDay][j] = courseName;
            }
        }
        courseMap[courseName].push_back("第" + examWeek + "周 " + examDayOfWeek + " 第" + examPeriodRange + "节 考试");
    }
}

Schedule::~Schedule()
{
}

string Schedule::getcourses(int week, int day, int period){
    return courses[week-1][day][period-1];
}

string Schedule::getexams(int week, int day, int period){
    return exam[week-1][day][period-1];
}


QString Schedule::FindCourse(const string &course)
{
    QString result;
    if (courseMap.count(course) == 0)
    {
        result = "查找失败，未找到课程";
    }
    else
    {
        result = QString::fromStdString(course) + "的课程信息如下：\n";
        for (const auto &info : courseMap[course]) // 遍历哈希表
        {
            result += QString::fromStdString(info) + "\n";
        }
    }
    return result;
}

void Schedule::DeleteCourse(int startWeek, int endWeek, int day, int startPeriod, int endPeriod)
{
    qDebug() << startWeek << endWeek << day << startPeriod << endPeriod;
    string course = courses[startWeek - 1][day - 1][startPeriod - 1]; // 获取课程名字
    qDebug() << QString::fromStdString(course);
    //qDebug() << startWeek << day << startPeriod;
    if (course == "")

    {
        qDebug() << "删除失败，未找到课程";
        return;
    }
    for (int i = startWeek - 1; i < endWeek; i++) // 清空课程表
    {
        for (int j = startPeriod - 1; j < endPeriod; j++)
        {
            courses[i][day - 1][j] = "";
        }
    }
    int examTime = stoi(courseMap[course][3].substr(courseMap[course][3].find("第") + strlen("第"), courseMap[course][3].find("周") - courseMap[course][3].find("第") - strlen("第")));
    std::size_t first = courseMap[course][3].find("周");
    std::size_t second = courseMap[course][3].find("周", first + 1);
    int examDay = day_to_int[courseMap[course][3].substr(second, strlen("周") + strlen("一"))];
    int examStartPeriod = stoi(courseMap[course][3].substr(courseMap[course][3].find('-') - 1));
    int examEndPeriod = stoi(courseMap[course][3].substr(courseMap[course][3].find('-') + 1));

    for (int i = examTime - 1; i < examTime; i++) // 清空考试表
    {
        for (int j = examStartPeriod - 1; j < examEndPeriod; j++)
        {
            exam[i][examDay - 1][j] = "";
        }
    }
    courseMap.erase(course); // 从哈希表中删除课程

    ifstream inFile("courses.txt");
    vector<string> lines;
    string line;

    // 读取文件中的所有内容并存储在字符串向量中
    while (getline(inFile, line)) {
        lines.push_back(line);
    }
    inFile.close();

    // 打开相同文件以进行写入，这将清除其内容
    ofstream outFile("courses.txt");

    for (const string &line : lines) {
        stringstream ss(line);
        string courseName;
        string weekRange;
        string dayOfWeek;
        string periodRange;
        string teacherName;
        string classroom;
        string examWeek;
        string examDayOfWeek;
        string examPeriodRange;
        string examRoom;
        getline(ss, courseName, ' ');
        getline(ss, weekRange, ' ');
        getline(ss, dayOfWeek, ' ');
        getline(ss, periodRange, ' ');
        getline(ss, teacherName, ' ');
        getline(ss, classroom, ' ');
        getline(ss, examWeek, ' ');
        getline(ss, examDayOfWeek, ' ');
        getline(ss, examPeriodRange, ' ');
        getline(ss, examRoom);

        int startW = stoi(weekRange.substr(0, weekRange.find('-')));
        int endW = stoi(weekRange.substr(weekRange.find('-') + 1));
        int d = day_to_int[dayOfWeek.substr(dayOfWeek.find("周"), strlen("周") + strlen("一"))];
        int startP = stoi(periodRange.substr(0, periodRange.find('-')));
        int endP = stoi(periodRange.substr(periodRange.find('-') + 1));

        // 检查当前课程是否基于给定条件与要删除的课程匹配
        if (courseName == course &&
            d == day - 1 &&
            startP == startPeriod && endP == endPeriod) {
            // 跳过写入文件的行
            continue;
        }

        // 如果课程名称匹配但时间不匹配，则保留该课程
        // 按照之前的方式将其数据存储在 courseMap 中
        else // 同名，不同时间的课程保留
        {
            courseMap[courseName].push_back("第" + weekRange + "周 " + dayOfWeek + " 第" + periodRange + "节 上课");
            courseMap[courseName].push_back("教室：" + classroom);
            courseMap[courseName].push_back("教师：" + teacherName);
            courseMap[courseName].push_back("第" + examWeek + "周 " + examDayOfWeek + " 第" + examPeriodRange + "节 考试");
        }

        // 将修改后的行写入文件
        outFile << line << endl;
    }

    outFile.close();
}

bool Schedule::checkCourseConflict(int start_week, int end_week, int day, int start_period, int end_period)
{
    for (int i = start_week - 1; i < end_week; i++)
    {
        for (int j = start_period - 1; j < end_period; j++)
        {
            if (i <= 15) // 16周之前是上课
            {
                if (courses[i][day-1][j] != "")
                    return true;
            }
            else
            {
                if (exam[i][day-1][j] != "")
                    return true;
            }
        }
    }
    return false;
}


void Schedule::setActivitySchedule(ActivitySchedule *activitySchedule) {
    activity_schedule = activitySchedule;
}

void Schedule::setTemporarySchedule(TemporarySchedule *tempSchedule) {
    temp_schedule = tempSchedule;
}

bool Schedule::checkOtherConflict(int start_week, int end_week, int day, int start_period, int end_period)
{
    int course_start = start_period + 1, course_end = end_period + 1; // 数组下标调整
    for (int i = start_week - 1; i < end_week; i++)
    {
        for (int j = course_start; j <= course_end; j++)
        {
            if (activity_schedule->team_activity[i][day-1][j] != "" || activity_schedule->personal_activity[i][day-1][j] != "" || temp_schedule->temp_things[i][day-1][j] != "")
            {
                qDebug() << i << day - 1 << j;
                qDebug() << QString::fromStdString(activity_schedule->team_activity[i][day-1][j]);
                qDebug() << QString::fromStdString(activity_schedule->personal_activity[i][day-1][i]);
                return true;
            }
        }
    }
    return false;
}


void Schedule::AddCourse(int startWeek, int endWeek, int day, int startPeriod, int endPeriod, string course, string teacherName, string classroom, int examTime, int examDay, int examStartPeriod, int examEndPeriod, string examRoom)
{

    for (int i = startWeek - 1; i < endWeek; i++)
    {
        for (int j = startPeriod - 1; j < endPeriod; j++)
        {
            courses[i][day - 1][j] = course;
        }
    }
    courseMap[course].push_back("第" + to_string(startWeek) + "-" + to_string(endWeek) + "周 周" + int_to_day[day - 1] + " 第" + to_string(startPeriod) + "-" + to_string(endPeriod) + "节 上课");
    courseMap[course].push_back("教室：" + classroom);
    courseMap[course].push_back("教师：" + teacherName);
    for (int i = examTime - 1; i < examTime; i++)
    {
        for (int j = examStartPeriod - 1; j < examEndPeriod; j++)
        {
            exam[i][examDay - 1][j] = course;
        }
    }
    courseMap[course].push_back("第" + to_string(examTime) + "周 周" + int_to_day[examDay - 1] + " 第" + to_string(examStartPeriod) + "-" + to_string(examEndPeriod) + "节 考试");
    file.clear();
    file.seekp(0, ios::end); // 将文件指针移动到文件末尾
    file << course << " " << startWeek << "-" << endWeek << " 周" << int_to_day[day - 1] << " " << startPeriod << "-" << endPeriod << " " << teacherName << " " << classroom << " " << examTime << " 周" << int_to_day[examDay - 1] << " " << examStartPeriod << "-" << examEndPeriod << " " << examRoom << endl;
}


