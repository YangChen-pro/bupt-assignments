#include "alarmwindow.h"
#include <QTimer>
#include <QHeaderView>
#include <regex>
#include <algorithm>
#include <QMessageBox>
extern MyMap<string, int> day_to_int;

bool cmp(CLOCK_TIME::alarm x,CLOCK_TIME::alarm y)
{
    return x.weekNum<y.weekNum||(x.weekNum==y.weekNum&&x.week<y.week)||(x.weekNum==y.weekNum&&x.week==y.week&&x.hour<y.hour) ;
}

AlarmWindow::AlarmWindow(logTXT*logtxt)

{

    this->logtxt = logtxt;
    mainLayout = new QVBoxLayout(this);
    searchLayout = new QHBoxLayout();


    getAlarm = new QPushButton(this);
    getAlarm->setText("获取提醒列表");
    scrollArea = new QScrollArea(this);
    tableWidget = new QTableWidget(scrollArea);
    tableWidget->setRowCount(100); // 设置表格的行数
    tableWidget->setColumnCount(4); // 设置表格的列数
    tableWidget->setItem(0, 0, new QTableWidgetItem("提醒序号"));
    tableWidget->setItem(0, 1, new QTableWidgetItem("活动名字"));
    tableWidget->setItem(0, 2, new QTableWidgetItem("活动时间"));
    tableWidget->setItem(0, 3, new QTableWidgetItem("地点/链接"));
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->resizeColumnsToContents();
    tableWidget->resizeRowToContents(2);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //隐藏行列号
    QHeaderView *verticalHeader = new QHeaderView(Qt::Orientation::Vertical);
    QHeaderView *horizontalHeader = new QHeaderView(Qt::Orientation::Horizontal);
    verticalHeader->setVisible(false);
    horizontalHeader->setVisible(false);
    tableWidget->setVerticalHeader(verticalHeader);
    tableWidget->setHorizontalHeader(horizontalHeader);

    scrollArea->setWidget(tableWidget);
    scrollArea->setWidgetResizable(true);

    searchLayout->addWidget(scrollArea);


    addLayout = new QHBoxLayout();
    btnAddAlarm = new QPushButton("添加提醒", this);
    addLayout->addWidget(btnAddAlarm);
    btnDelAlarm = new QPushButton("删除提醒", this);
    addLayout->addWidget(btnDelAlarm);
    btnChangeAlarm = new QPushButton("修改提醒", this);
    addLayout->addWidget(btnChangeAlarm);
    addLayout->addWidget(getAlarm);

    mainLayout->addLayout(searchLayout);

    mainLayout->addLayout(addLayout);


    connect(getAlarm, &QPushButton::clicked,this,[=](){
        this->logtxt->log("用户获取提醒列表");
        int len = this->logtxt->clk_time->alarm_list.size();
        sort(this->logtxt->clk_time->alarm_list.begin(),this->logtxt->clk_time->alarm_list.end(),cmp);


        tableWidget->setRowCount(len+1);
        for(int i=0;i<len;i++){
            tableWidget->setItem(i+1, 0, new QTableWidgetItem(QString::number(i+1)));
            tableWidget->setItem(i+1, 1, new QTableWidgetItem(QString::fromStdString(this->logtxt->clk_time->alarm_list[i].name)));
            tableWidget->setItem(i+1, 2, new QTableWidgetItem(QString::fromStdString(to_string(this->logtxt->clk_time->alarm_list[i].weekNum)+this->logtxt->clk_time->weekTostring(this->logtxt->clk_time->alarm_list[i].week)+this->logtxt->clk_time->hourTostring(this->logtxt->clk_time->alarm_list[i].hour))));
            tableWidget->setItem(i+1, 3, new QTableWidgetItem(QString::fromStdString(this->logtxt->clk_time->alarm_list[i].place_link)));
        }


    });
    connect(btnAddAlarm,&QPushButton::clicked,this,&AlarmWindow::addClicked);
    connect(btnDelAlarm,&QPushButton::clicked,this,&AlarmWindow::delClicked);
    connect(btnChangeAlarm,&QPushButton::clicked,this,&AlarmWindow::changeClicked);




}
void AlarmWindow::addClicked()
{
    QDialog addDialog(this);
    addDialog.setWindowTitle(tr("添加提醒"));
    QFormLayout formLayout(&addDialog);
    this->logtxt->log("用户添加提醒");

    QLineEdit edtActivity(tr("名称"), &addDialog);
    QLineEdit edtClassroom(tr("地点"), &addDialog);

    QSpinBox spbWeek(&addDialog);
    QSpinBox spbDay(&addDialog);
    QSpinBox spbTime(&addDialog);


    spbWeek.setRange(1,19);
    spbDay.setRange(1,7);
    spbTime.setRange(6,22);

    formLayout.addRow(tr("活动名称："), &edtActivity);

    formLayout.addRow(tr("活动地点："), &edtClassroom);
    formLayout.addRow(tr("周数："), &spbWeek);
    formLayout.addRow(tr("星期几："), &spbDay);
    formLayout.addRow(tr("时间"), &spbTime);
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &addDialog);
    formLayout.addRow(&buttonBox);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &addDialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &addDialog, &QDialog::reject);
    int result = addDialog.exec();
    if (result == QDialog::Accepted) {
    int flag = 1;
    std::string name = edtActivity.text().toStdString();
    std::string classroom = edtClassroom.text().toStdString();

    int Week = spbWeek.value();
    int day = spbDay.value();
    int time = spbTime.value();
    if(Week<1||day<0||day>=7||time<0||time>23)
    {
        flag = 0;
        QMessageBox::warning(this,tr("警告"),tr("添加失败"));
    }
    this->logtxt->clk_time->addAlarm(name,Week,day-1,time,classroom);
    if(flag)
    QMessageBox::information(this,tr("添加结果"),tr("添加成功"));

    }

}

void AlarmWindow::delClicked()
{
    QDialog delDialog(this);
    delDialog.setWindowTitle(tr("删除提醒"));
    QFormLayout formLayout(&delDialog);
    this->logtxt->log("用户删除提醒");
    QSpinBox spbInt(&delDialog);
    spbInt.setMinimum(1);
    formLayout.addRow(tr("活动序号："), &spbInt);
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &delDialog);
    formLayout.addRow(&buttonBox);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &delDialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &delDialog, &QDialog::reject);

    int result = delDialog.exec();
    if (result == QDialog::Accepted){

        int x= spbInt.value();
    if (x > 0 && x <= this->logtxt->clk_time->alarm_list.size())
        {
        this->logtxt->clk_time->alarm_list.erase(x-1 + this->logtxt->clk_time->alarm_list.begin());
        QMessageBox::information(this,tr("删除结果"),tr("删除成功"));
        }
        else{
        QMessageBox::information(this,tr("删除结果"),tr("删除失败"));
        }
    }
}

void AlarmWindow::changeClicked()
{

    QDialog changeDialog(this);
    changeDialog.setWindowTitle(tr("修改提醒"));
    QFormLayout formLayout(&changeDialog);
    this->logtxt->log("用户修改提醒");

    QSpinBox edtNum(&changeDialog);
    QLineEdit edtActivity(tr("名称"), &changeDialog);
    QLineEdit edtClassroom(tr("地点"), &changeDialog);

    QSpinBox spbWeek(&changeDialog);
    QSpinBox spbDay(&changeDialog);
    QSpinBox spbTime(&changeDialog);

    edtNum.setMinimum(1);
    spbWeek.setRange(1,19);
    spbDay.setRange(1,7);
    spbTime.setRange(6,22);

    formLayout.addRow(tr("序号："), &edtNum);
    formLayout.addRow(tr("活动名称："), &edtActivity);

    formLayout.addRow(tr("活动地点："), &edtClassroom);
    formLayout.addRow(tr("周数："), &spbWeek);
    formLayout.addRow(tr("星期几："), &spbDay);
    formLayout.addRow(tr("时间"), &spbTime);
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &changeDialog);
    formLayout.addRow(&buttonBox);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &changeDialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &changeDialog, &QDialog::reject);
    int result =changeDialog.exec();
    if (result == QDialog::Accepted) {
    int flag = 1;
    int num = edtNum.value();
    std::string name = edtActivity.text().toStdString();
    std::string classroom = edtClassroom.text().toStdString();

    int Week = spbWeek.value();
    int day = spbDay.value();
    int time = spbTime.value();
    if(Week<1||day<0||day>=7||time<0||time>23||num<=0||num>this->logtxt->clk_time->alarm_list.size())
    {
        flag = 0;
        QMessageBox::warning(this,tr("警告"),tr("修改失败"));
    }
    this->logtxt->clk_time->changeAlarm(num,name,Week,day-1,time,classroom);
    if(flag)
    QMessageBox::information(this,tr("修改结果"),tr("修改成功"));

    }



}

AlarmWindow::~AlarmWindow()
{
    this->logtxt->clk_time->alarm_list.~vector();
    this->logtxt->clk_time->now_alarm_list.~vector();
}


struct CourseInfo {
    int startWeek = 0;
    int endWeek = 0;
    string weekday = "";
    int startClass = 0;
    int endClass = 0;
    string classroom = "";
    int examWeek = 0;
    string examWeekday = "";
    int startExam = 0;
    int endExam = 0;
};

std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

std::string getWeekday(const std::wstring& input) {
    if (input.find(L"一") != std::wstring::npos) return "周一";
    if (input.find(L"二") != std::wstring::npos) return "周二";
    if (input.find(L"三") != std::wstring::npos) return "周三";
    if (input.find(L"四") != std::wstring::npos) return "周四";
    if (input.find(L"五") != std::wstring::npos) return "周五";
    if (input.find(L"六") != std::wstring::npos) return "周六";
    if (input.find(L"日") != std::wstring::npos) return "周日";
    return "";
}

CourseInfo extractCourseInfo(const std::vector<std::string>& courseStrings) {
    CourseInfo info;
    for (const auto& courseString : courseStrings) {
        std::wstring wCourseString = converter.from_bytes(courseString);
        std::wistringstream iss(wCourseString);
        std::wstring token;
        while (iss >> token) {
            if (token.substr(0, 1) == L"第" && wCourseString.find(L"上课") != std::wstring::npos) {
                size_t dashPos = token.find(L'-');
                size_t weekPos = token.find(L'周');
                info.startWeek = std::stoi(converter.to_bytes(token.substr(1, dashPos - 1)));
                info.endWeek = std::stoi(converter.to_bytes(token.substr(dashPos + 1, weekPos - dashPos - 1)));
                iss >> token; // 读取 "周三"
                info.weekday = getWeekday(token);
                iss >> token; // 读取 "第3-4节"
                dashPos = token.find(L'-');
                weekPos = token.find(L'节');
                info.startClass = std::stoi(converter.to_bytes(token.substr(1, dashPos - 1)));
                info.endClass = std::stoi(converter.to_bytes(token.substr(dashPos + 1, weekPos - dashPos - 1)));
            } else if (token.substr(0, 3) == L"教室：") {
                info.classroom = converter.to_bytes(token.substr(3));
            } else if (token.substr(0, 1) == L"第" && wCourseString.find(L"考试") != std::wstring::npos) {
                size_t weekPos = token.find(L'周');
                info.examWeek = std::stoi(converter.to_bytes(token.substr(1, weekPos - 1)));
                iss >> token; // 读取 "周一"
                info.examWeekday = getWeekday(token);
                iss >> token; // 读取 "第1-2节"
                size_t dashPos = token.find(L'-');
                weekPos = token.find(L'节');
                info.startExam = std::stoi(converter.to_bytes(token.substr(1, dashPos - 1)));
                info.endExam = std::stoi(converter.to_bytes(token.substr(dashPos + 1, weekPos - dashPos - 1)));
            }
        }
    }
    return info;
}

struct ActivityInfo{
    int startWeek;
    int endWeek;
    string weekday;
    int startTime;
    int endTime;
    string location;
};

ActivityInfo extractActivityInfo(const std::vector<std::string>& activityStrings) {
    ActivityInfo info;

    for (const auto& activityString : activityStrings) {
        std::wstring wActivityString = converter.from_bytes(activityString);
        std::wistringstream iss(wActivityString);
        std::wstring token;
        while (iss >> token) {
            if (token.substr(0, 1) == L"第" && wActivityString.find(L"周") != std::wstring::npos) {
                size_t dashPos = token.find(L'-');
                size_t weekPos = token.find(L'周');
                info.startWeek = std::stoi(converter.to_bytes(token.substr(1, dashPos - 1)));
                info.endWeek = std::stoi(converter.to_bytes(token.substr(dashPos + 1, weekPos - dashPos - 1)));
            } else if (token.substr(0, 1) == L"周") {
                info.weekday = getWeekday(token);
                iss >> token; // 读取 "6-8点"
                size_t dashPos = token.find(L'-');
                size_t pointPos = token.find(L'点');
                info.startTime = std::stoi(converter.to_bytes(token.substr(0, dashPos)));
                info.endTime = std::stoi(converter.to_bytes(token.substr(dashPos + 1, pointPos - dashPos - 1)));
            } else if (token.substr(0, 3) == L"地点：") {
                info.location = converter.to_bytes(token.substr(3));
            }
        }
    }

    return info;
}

struct TempInfo{
    int week;
    string weekday;
    int hour;
    int minute;
    string location;
};

TempInfo extractTempInfo(const std::vector<std::string>& tempStrings) {
    TempInfo info;

    for (const auto& tempString : tempStrings) {
        std::wstring wTempString = converter.from_bytes(tempString);
        std::wistringstream iss(wTempString);
        std::wstring token;
        while (iss >> token) {
            if (token.substr(0, 1) == L"第" && wTempString.find(L"周") != std::wstring::npos) {
                size_t weekPos = token.find(L'周');
                info.week = std::stoi(converter.to_bytes(token.substr(1, weekPos - 1)));
            } else if (token.substr(0, 1) == L"周") {
                info.weekday = getWeekday(token);
                iss >> token; // 读取 "3点45分"
                size_t pointPos = token.find(L'点');
                size_t minutePos = token.find(L'分');
                info.hour = std::stoi(converter.to_bytes(token.substr(0, pointPos)));
                info.minute = std::stoi(converter.to_bytes(token.substr(pointPos + 1, minutePos - pointPos - 1)));
            } else if (token.substr(0, 3) == L"地点：") {
                info.location = converter.to_bytes(token.substr(3));
            }
        }
    }

    return info;
}


void AlarmWindow::setAlarmlist()
{
    CLOCK_TIME::alarm a = {"",0,0,0,0,""};
    for (const auto& course : sche->courseMap)
    {
        CourseInfo info = extractCourseInfo(course.second);

//        course.first;
//        info.startWeek;
//        info.endWeek;
//        info.startClass;
//        info.endClass;
//        info.classroom;

//        info.examWeek;
//        info.examWeekday;
//        info.startExam;
//        info.endExam;

        for (int i = info.startWeek; i <= info.endWeek; i++)
        {
            //for (int j = info.startClass; j <= info.endClass; j++)
            {
                a.weekNum = i;
                a.week = day_to_int[info.weekday];
                a.name = course.first;
                a.hour = info.startClass + 7;
                a.place_link = info.classroom;

                this->logtxt->clk_time->addAlarm(a.name,a.weekNum,a.week,a.hour,a.place_link);
                //
            }
        }

        //for (int i = info.startExam; i <= info.endExam; i++)
        {
            a.weekNum = info.examWeek;
            a.week = day_to_int[info.examWeekday];
            a.name = course.first + "考试";
            a.hour = info.startExam + 7;
            a.place_link = info.classroom;
            this->logtxt->clk_time->addAlarm(a.name,a.weekNum,a.week,a.hour,a.place_link);
        }

    }

    for (const auto& activity : sche2->ActivityMap) {
        ActivityInfo info = extractActivityInfo(activity.second);

//        activity.first;
//        info.startWeek;
//        info.endWeek;
//        info.weekday;
//        info.startTime;
//        info.endTime;
//        info.location;

        for (int i = info.startWeek; i <= info.endWeek; i++)
        {
            //for (int j = info.startTime; j <= info.endTime; j++)
            {
                a.weekNum = i;
                a.week = day_to_int[info.weekday];
                a.name = activity.first;
                a.hour = info.startTime;
                a.place_link = info.location;
                this->logtxt->clk_time->addAlarm(a.name,a.weekNum,a.week,a.hour,a.place_link);
            }
        }
    }

    for (const auto& temp : sche3->TempMap) {
        TempInfo info = extractTempInfo(temp.second);
//        temp.first;
//        info.week;
//        info.weekday;
//        info.hour;
//        info.minute;
//        info.locatio;
        a.name = temp.first;
        a.weekNum = info.week;
        a.week = day_to_int[info.weekday];
        a.hour = info.hour;
        a.minute = info.minute;
        a.place_link = info.location;
        this->logtxt->clk_time->addAlarm(a.name,a.weekNum,a.week,a.hour,a.place_link);
    }

}
