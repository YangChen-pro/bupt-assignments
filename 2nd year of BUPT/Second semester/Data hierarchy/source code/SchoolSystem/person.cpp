#include "person.h"

extern MyMap<string, int> day_to_int;
extern MyMap<int, string> int_to_day;
Person::Person()
{
    memset(things, 0, sizeof(things));
    readUserAccounts();
    readCourses();
    readActivities();
    readTempEvents();
}

void Person::readUserAccounts() {

    ifstream infile("UserAccount.txt");
    string line;

    while (getline(infile, line)) {
        istringstream iss(line);
        string a, b, c;
        iss >> a >> b >> c;
        allName.push_back(c);
    }
}

void Person::readCourses()
{
    string filename = "courses.txt";
    fstream file;
    file.open(filename, ios::in);
    if (!file.is_open())
    {
        qDebug() << QString::fromStdString("打开文件失败: " + filename);
        return;
    }
    string line;

    while (getline(file, line)) {
        vector<string> tokens;
        size_t pos = 0;
        while ((pos = line.find(' ')) != string::npos) // 以空格分割字符串
        {
            tokens.push_back(line.substr(0, pos));
            line.erase(0, pos + 1);
        }
        tokens.push_back(line);
        int startWeek = stoi(tokens[1].substr(0, tokens[1].find('-')));
        int endWeek = stoi(tokens[1].substr(tokens[1].find('-') + 1));
        int day = day_to_int[tokens[2]];
        int startHour = stoi(tokens[3].substr(0, tokens[3].find('-')));
        startHour += 2; // 补偿时间差
        int endHour = stoi(tokens[3].substr(tokens[3].find('-') + 1));
        endHour += 2;

        int examWeek = stoi(tokens[6]);
        int examDay = day_to_int[tokens[7]];
        int examStartHour = stoi(tokens[8].substr(0, tokens[8].find('-')));
        examStartHour += 2;
        int examEndHour = stoi(tokens[8].substr(tokens[8].find('-') + 1));
        examEndHour += 2;

        for (int i = startWeek - 1; i < endWeek; ++i) {
            for (int j = startHour - 1; j < endHour; ++j) {
                things[i][day][j] += 1;
            }
        }
        for (int i = examStartHour - 1; i < examEndHour; i++)
            things[examWeek - 1][examDay][i] += 1;
    }
    file.close();
}

void Person::readActivities()
{
    for (int i = 0; i < allName.size(); i++)
    {
        string filename = allName[i] + "_activities.txt";
        fstream file;
        file.open(filename, ios::in);
        if (!file.is_open())
        {
            qDebug() << QString::fromStdString("打开文件失败: " + filename);
            return;
        }
        string line;

        while (getline(file, line)) {
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
            int startWeek = stoi(tokens[1].substr(0, tokens[1].find('-')));
            int endWeek = stoi(tokens[1].substr(tokens[1].find('-') + 1));
            int day = day_to_int[tokens[2]];
            int startHour = stoi(tokens[3].substr(0, tokens[3].find('-')));
            int endHour = stoi(tokens[3].substr(tokens[3].find('-') + 1));

            for (int i = startWeek - 1; i < endWeek; ++i) {
                for (int j = startHour - 1; j < endHour; ++j) {
                    things[i][day][j] += 1;
                }
            }
        }
        file.close();
    }
}

void Person::readTempEvents()
{
    for (int i = 0; i < allName.size(); i++)
    {
        string filename = allName[i] + "_tempevents.txt";
        fstream file;
        file.open(filename, ios::in);
        if (!file.is_open())
        {
            qDebug() << QString::fromStdString("打开文件失败: " + filename);
            return;
        }
        string line;

        while (getline(file, line)) {
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
            int week = stoi(tokens[1]);
            int day = day_to_int[tokens[2]];
            int hour = stoi(tokens[3]) - 6;

            things[week-1][day][hour] += 1;
        }
        file.close();
    }
}

