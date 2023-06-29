#ifndef CLOCK_TIME_H
#define CLOCK_TIME_H


#include <iostream>
#include <vector>
#include <QString>
using namespace std;

class CLOCK_TIME
{
public:

    int time_line;//按小时算

    bool isEND;
    bool flag;
    void END();
    void CONTINUE();
    void PAUSE();
    int getYear();
    int getMonth();
    int getDay();
    int getHour();
    int getWeekDay();//周几
    int getWeekNum();//第几周
    struct alarm
    {
        std::string name;
        int weekNum;//第几周
        int week;//周几
        int hour;
        int minute = 0;
        std::string place_link;//地点或链接
    };

    std::vector<alarm> alarm_list;
    std::vector<alarm> now_alarm_list;
    string now_location ="学三公寓";
    CLOCK_TIME();
    ~CLOCK_TIME();

    void alm_for_tonight();
    void alm_for_hour();
    void alm_for_compaign();
    //alarm* showAlarm();
    void addAlarm(string name,int weekNum,int week,int hour,string place_link);
    void delAlarm(int x);
    void changeAlarm(int x, string name,int weekNum,int week,int hour,string place_link);

    void counter();
    string weekTostring(int x);
    string hourTostring(int x);

};


#endif // CLOCK_TIME_H
