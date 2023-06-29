#include <ctime>
#include <iostream>
#include "CLOCK_TIME.h"
#include <QDebug>
#include <string>
using namespace std;

CLOCK_TIME::CLOCK_TIME()
{
    this->isEND = 1;
    this->time_line = 0;
    this->flag = 1;
    this->alarm_list = {};
}

CLOCK_TIME::~CLOCK_TIME()
{
    END();
}
void CLOCK_TIME::END()
{
    this->flag = 0;
    this->isEND = 0;
    this->time_line = 0;
}


void CLOCK_TIME::PAUSE()
{
    this->flag = 0;
    qDebug("计时暂停");
}
void CLOCK_TIME::CONTINUE()
{
    this->flag = 1;
    qDebug("计时继续");
}

int CLOCK_TIME::getHour()
{
    return time_line % 24;
}
int CLOCK_TIME::getDay()
{
    return (time_line / 24) % 30;
}
int CLOCK_TIME::getMonth()
{
    return (time_line / 720) % 12;
}
int CLOCK_TIME::getYear()
{
    return time_line / 8640;
}
int CLOCK_TIME::getWeekDay()
{
    return (time_line % 168) / 24;
}
int CLOCK_TIME::getWeekNum()
{
    return time_line / 168;
}

void CLOCK_TIME::alm_for_tonight()
{
    if (getHour() == 20)
    {
        for (int i = 0; i < alarm_list.size(); i++)
        {
            if ((this->alarm_list[i].week == (getWeekDay()+1)%7 && this->alarm_list[i].weekNum==this->getWeekNum()+1)||(alarm_list[i].week==0 && (alarm_list[i].weekNum==this->getWeekNum()+2)))
            {
                //cout << "明日提醒：" << alarm_list[i].name << " " << alarm_list[i].hour << " " << alarm_list[i].place_link << endl;
                this->now_alarm_list.push_back(alarm_list[i]);
            }
        }
    }
}
void CLOCK_TIME::alm_for_hour()
{
    if(getHour()!=8 && getHour()!=20){
        vector<alarm>alist;
        for (int i = 0; i < alarm_list.size(); i++)
        {
            if (alarm_list[i].week == getWeekDay() && (alarm_list[i].hour == getHour() + 1 )&&alarm_list[i].weekNum ==getWeekNum()+1)
            {
                //cout << "课程提醒：" << alarm_list[i].name << " " << alarm_list[i].hour << " " << alarm_list[i].place_link << endl;
                alist.push_back(alarm_list[i]);
            }this->now_alarm_list = alist;
        } //alist.~vector();
    }
    else{

        for (int i = 0; i < alarm_list.size(); i++)
        {
            if (alarm_list[i].week == getWeekDay() && alarm_list[i].hour == getHour() + 1&&alarm_list[i].weekNum ==getWeekNum()+1)
            {
                //cout << "课程提醒：" << alarm_list[i].name << " " << alarm_list[i].hour << " " << alarm_list[i].place_link << endl;
                this->now_alarm_list.push_back(alarm_list[i]);
            }
        }
    }
}
void CLOCK_TIME::alm_for_compaign()
{
    if (getHour() == 8)
    {
        for (int i = 0; i < alarm_list.size(); i++)
        {
            if (alarm_list[i].week == getWeekDay()&&alarm_list[i].weekNum ==getWeekNum()+1)
            {
                //cout << "活动提醒：" << alarm_list[i].name << " " << alarm_list[i].hour << " " << alarm_list[i].place_link << endl;
                this->now_alarm_list.push_back(alarm_list[i]);
            }
        }
    }
}
void CLOCK_TIME::addAlarm(string name,int weekNum,int week,int hour,string place_link)
{
    alarm alm;
    alm.name =name;
    alm.weekNum = weekNum;
    alm.week = week;
    alm.hour = hour;
    alm.place_link = place_link;
    this->alarm_list.push_back(alm);
}
void CLOCK_TIME::changeAlarm(int x, string name,int weekNum,int week,int hour,string place_link)
{

    alarm alm;
    alm.name = name;alm.week=week;alm.weekNum = weekNum;alm.place_link = place_link;
    alm.hour = hour;
    alarm_list[x-1] = alm;
}


void CLOCK_TIME::delAlarm(int x)
{
    alarm_list.erase(x + alarm_list.begin());
}

void CLOCK_TIME::counter()
{
    int delta = 0;
    time_t* now = new time_t;
    time_t* lastTime = new time_t;
    *lastTime = 0;
    while (isEND) {
        if (this->flag) {
            *now = clock() / CLOCKS_PER_SEC;
            if (*now > *lastTime)
            {
                *lastTime = *now;
                delta++;
                if (delta == 10)//
                {
                    delta = 0;
                    this->time_line++;
                    alm_for_tonight();
                    alm_for_hour();
                    alm_for_compaign();
                    // std::cout << "时间过了1h" << std::endl;
                    std::cout << getWeekNum() << " " << getWeekDay() << " " << getHour() << std::endl;

                }
            }

        }
    }	std::cout << "计时已结束" << std::endl;
}

string CLOCK_TIME::weekTostring(int x){
    switch(x){
    case 0:return "周一";
    case 1:return "周二";
    case 2:return "周三";
    case 3:return "周四";
    case 4:return "周五";
    case 5:return "周六";
    case 6:return "周日";
    }
}
string CLOCK_TIME::hourTostring(int x){
    switch(x){
    case 0:return "午夜零点";
    case 1:return "凌晨一点";
    case 2:return "凌晨两点";
    case 3:return "凌晨三点";
    case 4:return "凌晨四点";
    case 5:return "凌晨五点";
    case 6:return "早上六点";

    case 7:return "早上七点";
    case 8:return "早上八点";

    case 9:return "上午九点";

    case 10:return "上午十点";

    case 11:return "上午十一点";

    case 12:return "中午十二点";

    case 13:return "下午一点";
    case 14:return "下午两点";
    case 15:return "下午三点";
    case 16:return "下午四点";
    case 17:return "下午五点";
    case 18:return "下午六点";
    case 19:return "晚上七点";
    case 20:return "晚上八点";
    case 21:return "晚上九点";
    case 22:return "晚上十点";
    case 23:return "晚上十一点";
    }
}
