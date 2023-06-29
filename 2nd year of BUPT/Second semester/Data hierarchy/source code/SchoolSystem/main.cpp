#include <QApplication>
#include "loginwindow.h"
#include "schedulewindow.h"
#include "activitywindow.h"
#include "tempwindow.h"
#include "adminwindow.h"
#include "mymap.h"
#include "log.h"
#include <unordered_map>
#include <string>
#include <thread>

void thread_time(CLOCK_TIME *clk_time)
{
    clk_time->counter();
}
MyMap<string, int> day_to_int =
{
{"周一", 0},
{"周二", 1},
{"周三", 2},
{"周四", 3},
{"周五", 4},
{"周六", 5},
{"周日", 6}};
MyMap<int, string> int_to_day =
{
{0, "一"},
{1, "二"},
{2, "三"},
{3, "四"},
{4, "五"},
{5, "六"},
{6, "日"}};

int main(int argc, char *argv[])
{

    CLOCK_TIME clk_time= CLOCK_TIME();
    logTXT logtxt = logTXT("log.txt", &clk_time);

    thread TimeLine(thread_time, &clk_time);
    TimeLine.detach();

    QApplication app(argc, argv);

    LoginWindow loginWindow = LoginWindow(&logtxt);


    loginWindow.show();


//    AdminWindow a = AdminWindow(&logtxt);
//    a.show();

//    StudentWindow s =StudentWindow(&logtxt,"2021212171");
//    s.show();

//    ScheduleWindow w(&logtxt,"courses.txt");
//    w.show();
//    ActivityWindow s(&logtxt,"ZhangSan_activities.txt");
//    s.show();



    return app.exec();
}
