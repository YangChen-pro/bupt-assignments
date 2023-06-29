#include "studentwindow.h"
//#include "schedulewindow.h"
//#include "activitywindow.h"
//#include "tempwindow.h"
//#include "alarmwindow.h"




StudentWindow::StudentWindow(logTXT*logtxt,const QString &username, QWidget *parent) : QWidget(parent)
{

    this->logtxt = logtxt;
    studentLabel = new QLabel("学生", this); // 重新初始化 studentLabel
    nameNumberLabel = new QLabel(this); // 初始化新添加的 QLabel

    // 读取 UserAccount.txt 文件，获取当前登录学生的姓名和学号
    QString name;
    QString number;
    QFile userFile("UserAccount.txt");
    if (userFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&userFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList userInfo = line.split(" ");
            if (userInfo.size() >= 3 && userInfo[0] == username) {
                number = userInfo[0];
                name = userInfo[2];
                break;
            }
        }
        userFile.close();
    }

    nameNumberLabel->setText("姓名: " + name + " 学号: " + number); // 设置 QLabel 的文本为学生的姓名和学号
    labelTime = new QLineEdit("这里是时间",this);


    this->today = this->logtxt->clk_time->getDay();

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        string time ="第"+to_string(this->logtxt->clk_time->getWeekNum()+1)+"周"+ this->logtxt->clk_time->weekTostring(this->logtxt->clk_time->getWeekDay())+this->logtxt->clk_time->hourTostring(this->logtxt->clk_time->getHour());
        labelTime->setText(QString::fromStdString(time));
        if (this->logtxt->clk_time->getDay() != this->today)
        {
            this->today++;
            this->logtxt->clk_time->now_location = "学三公寓";
        }
        int leng = this->logtxt->clk_time->now_alarm_list.size();
        tableWidgetAlarm->setRowCount(leng+1);
        for(int i=0;i<leng;i++){
            tableWidgetAlarm->setItem(i+1, 0, new QTableWidgetItem(QString::fromStdString(this->logtxt->clk_time->now_alarm_list[i].name)));
            tableWidgetAlarm->setItem(i+1, 1, new QTableWidgetItem(QString::fromStdString("第"+to_string(this->logtxt->clk_time->getWeekNum()+1)+"周"+this->logtxt->clk_time->weekTostring(this->logtxt->clk_time->now_alarm_list[i].week)+this->logtxt->clk_time->hourTostring(this->logtxt->clk_time->now_alarm_list[i].hour))));
            tableWidgetAlarm->setItem(i+1, 2, new QTableWidgetItem(QString::fromStdString(this->logtxt->clk_time->now_alarm_list[i].place_link)));
        }
    });



    timer->start(2000); // 每2秒更新一次文本框的内容

    pause = new QPushButton("暂停/继续计时",this);
    myLabel = new  QLabel("",this);  // 创建文本框对象，并指定父对象为当前窗口
    myLabel->setStyleSheet("background-color: yellow; border: 2px solid write;");
    scrollAreaAlarm = new QScrollArea(this);
    tableWidgetAlarm = new QTableWidget();
    tableWidgetAlarm->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidgetAlarm->setRowCount(10);
    tableWidgetAlarm->setColumnCount(3);
    tableWidgetAlarm->setItem(0, 0, new QTableWidgetItem("活动名字"));
    tableWidgetAlarm->setItem(0, 1, new QTableWidgetItem("活动时间"));
    tableWidgetAlarm->setItem(0, 2, new QTableWidgetItem("地点/链接"));


    scrollAreaAlarm->setWidget(tableWidgetAlarm);
    scrollAreaAlarm->setWidgetResizable(true);


    bool p_c = true;
    connect(pause, &QPushButton::clicked, this, [this, p_c]() mutable {
        if (p_c) {
            this->logtxt->clk_time->PAUSE();
            myLabel->setText("计时已暂停");
            this->logtxt->log("用户暂停计时");
        } else {
            this->logtxt->clk_time->CONTINUE();
            myLabel->setText("计时继续");
            this->logtxt->log("用户继续计时");
        }
        p_c = !p_c;
    });

    menuListWidget = new QListWidget(this);
    // 为菜单列表部件设置样式表
    menuListWidget->setStyleSheet("QListWidget {background-color: #EDEDED; border: 1px solid #CCCCCC;} QListWidget::item:selected {background-color: #5F87D6; color: white;} QListWidget::item:hover {background-color: #7198E8; color: white;}");

    menuListWidget->addItem("查看课程表");
    menuListWidget->addItem("查看课外活动");
    menuListWidget->addItem("查看临时事务");
    menuListWidget->addItem("查看提醒表");
    menuListWidget->addItem("导航");

    stackedWidget = new QStackedWidget(this);


    // 创建 ScheduleWindow 实例并将其添加到 stackedWidget
    ScheduleWindow *scheduleWindow = new ScheduleWindow(this->logtxt,"courses.txt");
    stackedWidget->addWidget(scheduleWindow);

    // 创建 ActivityWindow 实例并将其添加到 stackedWidget
    ActivityWindow *activityWindow = new ActivityWindow(this->logtxt,name.toStdString()+"_activities.txt");
    stackedWidget->addWidget(activityWindow);

    // 创建 TempWindow 实例并将其添加到 stackWidget
    TempWindow *tempWindow = new TempWindow(this->logtxt,name.toStdString() + "_tempevents.txt");
    stackedWidget->addWidget(tempWindow);

    AlarmWindow *alarmWindow = new AlarmWindow(this->logtxt);
    stackedWidget->addWidget(alarmWindow);


    alarmWindow->sche = scheduleWindow->schedule;
    alarmWindow->sche2 = activityWindow->activity_schedule;
    alarmWindow->sche3 = tempWindow->Temp_schedule;
    alarmWindow->setAlarmlist();

    // 3者相互访问 指针互指
    scheduleWindow->schedule->setActivitySchedule(activityWindow->activity_schedule);
    scheduleWindow->schedule->setTemporarySchedule(tempWindow->Temp_schedule);

    activityWindow->activity_schedule->setSchedule(scheduleWindow->schedule);
    activityWindow->activity_schedule->setTemporarySchedule(tempWindow->Temp_schedule);

    tempWindow->Temp_schedule->setSchedule(scheduleWindow->schedule);
    tempWindow->Temp_schedule->setActivitySchedule(activityWindow->activity_schedule);


    NavigateWindow *navigateWindow = new NavigateWindow(this->logtxt);
    stackedWidget->addWidget(navigateWindow);


    // 将信号连接到 stackedWidget 的 setCurrentIndex 槽
    connect(menuListWidget, &QListWidget::currentRowChanged, stackedWidget, &QStackedWidget::setCurrentIndex);


    QVBoxLayout *leftTopLayout = new QVBoxLayout(); // 新建一个垂直布局
    leftTopLayout->addWidget(pause);
    leftTopLayout->addWidget(labelTime);
    leftTopLayout->addWidget(myLabel);
    leftTopLayout->addWidget(studentLabel); // 将 studentLabel 添加到垂直布局中
    leftTopLayout->addWidget(nameNumberLabel); // 将新添加的 QLabel 添加到垂直布局中


    QVBoxLayout *leftLayout = new QVBoxLayout(); // 新建一个垂直布局
    leftLayout->addLayout(leftTopLayout); // 将 leftTopLayout 添加到垂直布局中

    leftLayout->addWidget(menuListWidget); // 将菜单列表部件添加到垂直布局中
    leftLayout->addWidget(scrollAreaAlarm);


    mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(leftLayout); // 将垂直布局添加到主布局中


    mainLayout->addWidget(stackedWidget);

    //connect(menuListWidget, &QListWidget::currentRowChanged, stackedWidget, &QStackedWidget::setCurrentIndex);
}


StudentWindow::~StudentWindow()
{
    this->logtxt->clk_time->alarm_list.~vector();
    this->logtxt->clk_time->now_alarm_list.~vector();
}
