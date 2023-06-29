#include "schedulewindow.h"
#include "schedule.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QTableWidget>
#include <QLabel>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QHeaderView>
#include <QTimer>

ScheduleWindow::ScheduleWindow(logTXT*logtxt,string fileName, QWidget *parent)
    : QWidget(parent)
{
    this->logtxt = logtxt;

    // 添加周数标签
    weekLabel = new QLabel(this);

    weekLabel->setText("第1周");

   // this->weekNum = logtxt->clk_time->getWeekNum();
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        this->weekNum = this->logtxt->clk_time->getWeekNum() + 1;
        weekLabel->setText(QString::fromStdString("第"+to_string(this->weekNum)+"周"));
    });
    timer->start(2000);


    schedule = new Schedule("courses.txt");


    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(weekLabel);

    mainLayout->addWidget(weekLabel);


    searchLayout = new QHBoxLayout();
    searchInput = new QLineEdit(this);
    btnFindCourse = new QPushButton("查找课程", this);
    searchLayout->addWidget(searchInput);
    searchLayout->addWidget(btnFindCourse);

    findResultDisplay = new QTextEdit(this);
    findResultDisplay->setReadOnly(true);
    mainLayout->addWidget(findResultDisplay);

    addLayout = new QHBoxLayout();
    btnAddCourse = new QPushButton("添加课程", this);
    addLayout->addWidget(btnAddCourse);

    createCourseTable();
    updateCourseTable();

    mainLayout->addLayout(searchLayout);
    mainLayout->addWidget(scheduleDisplay);
    mainLayout->addLayout(addLayout);

    connect(btnFindCourse, &QPushButton::clicked, this, &ScheduleWindow::onFindCourseClicked);
    connect(btnAddCourse, &QPushButton::clicked, this, &ScheduleWindow::onAddCourseClicked);
}

ScheduleWindow::~ScheduleWindow()
{
    delete schedule;
    this->logtxt->clk_time->alarm_list.~vector();
    this->logtxt->clk_time->now_alarm_list.~vector();
}

void ScheduleWindow::createCourseTable()
{
    // 创建表格
    scheduleDisplay = new QTableWidget(13, 7, this);
    scheduleDisplay->setEditTriggers(QAbstractItemView::NoEditTriggers);
    scheduleDisplay->setSelectionMode(QAbstractItemView::SingleSelection);

    // 设置表格行标题
    QStringList rowLabels;
    for (int i = 8; i < 21; ++i)
    {
        rowLabels << QString("%1:00~%2:00").arg(QString::number(i), 2, '0').arg(QString::number(i+1), 2, '0');
    }
    scheduleDisplay->setVerticalHeaderLabels(rowLabels);

    // 设置表格列标题
    QStringList columnLabels;
    columnLabels << "周一" << "周二" << "周三" << "周四" << "周五" << "周六" << "周日";
    scheduleDisplay->setHorizontalHeaderLabels(columnLabels);

    // 连接单元格点击信号
    connect(scheduleDisplay, &QTableWidget::cellClicked, this, &ScheduleWindow::onCourseCellClicked);
}

void ScheduleWindow::updateCourseTable()
{
    // 更新周数标签
    //weekLabel->setText(tr("第 %1 周").arg(weekNum));

    // 更新表格内容
    for (int row = 0; row < 13; ++row)
    {
        for (int col = 0; col < 7; ++col)
        {
            QString text;
            if (weekNum <= 16)
            {
                text = QString::fromStdString(schedule->getcourses(weekNum,col,row+1));
            }
            else
            {
                text = QString::fromStdString(schedule->getexams(weekNum,col,row+1));
            }
            QTableWidgetItem *item = new QTableWidgetItem(text);
            scheduleDisplay->setItem(row, col, item);
        }
    }
    scheduleDisplay->resizeColumnsToContents();
    scheduleDisplay->resizeRowsToContents();
}


void ScheduleWindow::onFindCourseClicked()
{
    QString courseName = searchInput->text();
    QString findResult = schedule->FindCourse(courseName.toStdString());
    this->logtxt->log("用户查找课程："+courseName.toStdString());
    findResultDisplay->setText(findResult);
}


void ScheduleWindow::onAddCourseClicked()
{
    // 创建一个QDialog窗口，命名为addDialog，作为本窗口的子窗口
    QDialog addDialog(this);
    addDialog.setWindowTitle(tr("添加课程"));
    QFormLayout formLayout(&addDialog);

    // 创建用于输入课程信息的控件（QLineEdit和QSpinBox）
    QLineEdit edtCourse(tr("课程名称"), &addDialog);
    QLineEdit edtTeacherName(tr("教师姓名"), &addDialog);
    QLineEdit edtClassroom(tr("教室"), &addDialog);
    QSpinBox spbStartWeek(&addDialog);
    QSpinBox spbEndWeek(&addDialog);
    QSpinBox spbDay(&addDialog);
    QSpinBox spbStartPeriod(&addDialog);
    QSpinBox spbEndPeriod(&addDialog);
    QSpinBox spbExamTime(&addDialog);
    QSpinBox spbExamDay(&addDialog);
    QSpinBox spbExamStartPeriod(&addDialog);
    QSpinBox spbExamEndPeriod(&addDialog);
    QLineEdit edtExamRoom(tr("考试教室"), &addDialog);

    // 设置QSpinBox的范围
    spbStartWeek.setRange(1, 16);
    spbEndWeek.setRange(1, 16);
    spbDay.setRange(1, 7);
    spbStartPeriod.setRange(1, 13);
    spbEndPeriod.setRange(1, 13);
    spbExamTime.setRange(1, 20);
    spbExamDay.setRange(1, 7);
    spbExamStartPeriod.setRange(1, 13);
    spbExamEndPeriod.setRange(1, 13);

    // 将控件添加到formLayout布局中，并设置标签文本
    formLayout.addRow(tr("课程名称："), &edtCourse);
    formLayout.addRow(tr("教师姓名："), &edtTeacherName);
    formLayout.addRow(tr("教室："), &edtClassroom);
    formLayout.addRow(tr("开始周："), &spbStartWeek);
    formLayout.addRow(tr("结束周："), &spbEndWeek);
    formLayout.addRow(tr("星期几上课："), &spbDay);
    formLayout.addRow(tr("开始时间（节）："), &spbStartPeriod);
    formLayout.addRow(tr("结束时间（节）："), &spbEndPeriod);
    formLayout.addRow(tr("考试时间（周）："), &spbExamTime);
    formLayout.addRow(tr("考试日期（星期几）："), &spbExamDay);
    formLayout.addRow(tr("考试开始时间（节）："), &spbExamStartPeriod);
    formLayout.addRow(tr("考试结束时间（节）："), &spbExamEndPeriod);
    formLayout.addRow(tr("考试教室："), &edtExamRoom);

    // 添加确认和取消按钮
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &addDialog);
    formLayout.addRow(&buttonBox);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &addDialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &addDialog, &QDialog::reject);

    // 显示对话框并等待用户操作
    int result = addDialog.exec();
    if (result == QDialog::Accepted) {
        // 用户点击了确认按钮，获取输入的课程信息
        std::string course = edtCourse.text().toStdString();
        std::string teacherName = edtTeacherName.text().toStdString();
        std::string classroom = edtClassroom.text().toStdString();
        int startWeek = spbStartWeek.value();
        int endWeek = spbEndWeek.value();
        int day = spbDay.value();
        int startPeriod = spbStartPeriod.value();
        int endPeriod = spbEndPeriod.value();
        int examTime = spbExamTime.value();
        int examDay = spbExamDay.value();
        int examStartPeriod = spbExamStartPeriod.value();
        int examEndPeriod = spbExamEndPeriod.value();
        std::string examRoom = edtExamRoom.text().toStdString();

        if (schedule->checkCourseConflict(startWeek, endWeek, day, startPeriod, endPeriod))
        {
            QMessageBox::warning(this, "添加失败", "此时间段里有其他课程");
            return;
        }

        for (int i = examStartPeriod; i <= examEndPeriod; i++)
        {
            if (schedule->getexams(examTime, examDay, i) != "")
            {
                QMessageBox::warning(this, "添加失败", "此时间段里有其他考试");
                return;
            }
        }
        if (schedule->checkOtherConflict(startWeek, endWeek, day, startPeriod, endPeriod))
            QMessageBox::warning(this, "警告", "此时间段里有其他活动或临时事务，请及时调整");

        // 调用AddCourse函数添加课程
        schedule->AddCourse(startWeek, endWeek, day, startPeriod, endPeriod, course, teacherName, classroom, examTime, examDay, examStartPeriod, examEndPeriod, examRoom);

        // 更新scheduleDisplay
        updateCourseTable();

        CLOCK_TIME::alarm bb;
        bb.name = course;
        bb.hour = startPeriod + 7;
        bb.week = day - 1;
        bb.place_link = classroom;
        for (int i = startWeek; i <= endWeek; i++)
        {
            bb.weekNum = i;
            this->logtxt->clk_time->alarm_list.push_back(bb);
        }
        bb.name = course + "考试";
        bb.weekNum = examTime;
        bb.week = examDay - 1;
        bb.hour = examStartPeriod + 7;
        bb.place_link = examRoom;
        this->logtxt->clk_time->alarm_list.push_back(bb);

    }
}

void ScheduleWindow::onCourseCellClicked(int row, int col)
{
    QTableWidgetItem *item = scheduleDisplay->item(row, col);
    this->logtxt->log("用户点击课程 第"+to_string(row+1)+"行 第"+to_string(col+1)+"列");
    QString courseName = item->text();
    if (courseName.isEmpty()|| weekNum >= 16)
    {
        return;
    }

    QDialog dialog(this);
    QVBoxLayout layout(&dialog);

    QLabel label("你想做什么？", &dialog);
    layout.addWidget(&label);

    int startWeek = weekNum; // 当前周
    int endWeek = 16;
    int day = col + 1;
    int startPeriod = row + 1;

    // 寻找一次课程中需要删除的所有方格
    int endPeriod = row + 1;
    for (int i = row + 1; i < 14; i++) {
        QTableWidgetItem *nextItem = scheduleDisplay->item(i, col);
        if (nextItem->text() != courseName) {
            break;
        }
        endPeriod++;
    }


    QPushButton btnDelete("删除课程", &dialog);
    layout.addWidget(&btnDelete);
    connect(&btnDelete, &QPushButton::clicked, [&]() {
        this->logtxt->log("用户删除课程 第"+to_string(row+1)+"行 第"+to_string(col+1)+"列");
        schedule->DeleteCourse(startWeek, endWeek, day, startPeriod, endPeriod);
        updateCourseTable();
        dialog.close();

        for(int j=0;j<this->logtxt->clk_time->alarm_list.size();j++)
        {
            if(this->logtxt->clk_time->alarm_list[j].name == courseName.toStdString() || this->logtxt->clk_time->alarm_list[j].name == courseName.toStdString() + "考试")
            {
                this->logtxt->clk_time->alarm_list.erase(j+this->logtxt->clk_time->alarm_list.begin());
            }
        }
    });

    QPushButton btnEdit("修改课程", &dialog);
    layout.addWidget(&btnEdit);
    connect(&btnEdit, &QPushButton::clicked, [&]() {
        this->logtxt->log("用户修改课程 第"+to_string(row+1)+"行 第"+to_string(col+1)+"列");
        QDialog editDialog(&dialog);
        QFormLayout formLayout(&editDialog);

        QLineEdit edtCourse(tr("课程名称"), &editDialog);
        QLineEdit edtTeacherName(tr("教师姓名"), &editDialog);
        QLineEdit edtClassroom(tr("教室"), &editDialog);
        QSpinBox spbStartWeek(&editDialog);
        QSpinBox spbEndWeek(&editDialog);
        QSpinBox spbDay(&editDialog);
        QSpinBox spbStartPeriod(&editDialog);
        QSpinBox spbEndPeriod(&editDialog);
        QSpinBox spbExamTime(&editDialog);
        QSpinBox spbExamDay(&editDialog);
        QSpinBox spbExamStartPeriod(&editDialog);
        QSpinBox spbExamEndPeriod(&editDialog);
        QLineEdit edtExamRoom(tr("考试教室"), &editDialog);

        formLayout.addRow(tr("课程名称："), &edtCourse);
        formLayout.addRow(tr("教师姓名："), &edtTeacherName);
        formLayout.addRow(tr("教室："), &edtClassroom);
        formLayout.addRow(tr("开始周："), &spbStartWeek);
        formLayout.addRow(tr("结束周："), &spbEndWeek);
        formLayout.addRow(tr("星期几上课："), &spbDay);
        formLayout.addRow(tr("开始时间（节）："), &spbStartPeriod);
        formLayout.addRow(tr("结束时间（节）："), &spbEndPeriod);
        formLayout.addRow(tr("考试时间（周）："), &spbExamTime);
        formLayout.addRow(tr("考试日期（星期几）："), &spbExamDay);
        formLayout.addRow(tr("考试开始时间（节）："), &spbExamStartPeriod);
        formLayout.addRow(tr("考试结束时间（节）："), &spbExamEndPeriod);
        formLayout.addRow(tr("考试教室："), &edtExamRoom);

        spbStartWeek.setRange(1, 16);
        spbEndWeek.setRange(1, 16);
        spbDay.setRange(1, 7);
        spbStartPeriod.setRange(1, 13);
        spbEndPeriod.setRange(1, 13);
        spbExamTime.setRange(1, 20);
        spbExamDay.setRange(1, 7);
        spbExamStartPeriod.setRange(1, 13);
        spbExamEndPeriod.setRange(1, 13);

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &editDialog);
        formLayout.addRow(&buttonBox);
        connect(&buttonBox, &QDialogButtonBox::accepted, &editDialog, &QDialog::accept);
        connect(&buttonBox, &QDialogButtonBox::rejected, &editDialog, &QDialog::reject);

        if (editDialog.exec() == QDialog::Accepted)
        {
            int new_startWeek = spbStartWeek.value();
            int new_endWeek = spbEndWeek.value();
            int new_day = spbDay.value();
            int new_startPeriod = spbStartPeriod.value();
            int new_endPeriod = spbEndPeriod.value();
            int new_examTime = spbExamTime.value();
            int new_examDay = spbExamDay.value();
            int new_examStartPeriod = spbExamStartPeriod.value();
            int new_examEndPeriod = spbExamEndPeriod.value();
            string new_courseName = edtCourse.text().toStdString();
            string new_teacherName = edtTeacherName.text().toStdString();
            string new_classroom = edtClassroom.text().toStdString();
            string new_examRoom = edtExamRoom.text().toStdString();


            // 修改的实质是把旧的课程删除，把新的课程添加进来
            schedule->DeleteCourse(startWeek, endWeek, day, startPeriod, endPeriod);
            this->logtxt->log("修改内容"+new_courseName+new_classroom+new_teacherName+" "+to_string(new_startWeek)+"-"+to_string(new_endWeek)+"day"+to_string(day)+" "+to_string(new_startPeriod)+" "+to_string(new_endPeriod)+"考试"+
            new_examRoom+to_string(new_examStartPeriod)+"-"+to_string(new_examEndPeriod)+"考试时间"+to_string(new_examDay)+" "+to_string(new_examTime));
            schedule->AddCourse(new_startWeek, new_endWeek, new_day, new_startPeriod, new_endPeriod, new_courseName, new_teacherName, new_classroom, new_examTime, new_examDay, new_examStartPeriod, new_examEndPeriod, new_examRoom);

            updateCourseTable();

            for(int j=0;j<this->logtxt->clk_time->alarm_list.size();j++)
            {
                if(this->logtxt->clk_time->alarm_list[j].name == courseName.toStdString() || this->logtxt->clk_time->alarm_list[j].name == courseName.toStdString() + "考试")
                {
                    this->logtxt->clk_time->alarm_list.erase(j+this->logtxt->clk_time->alarm_list.begin());
                }
            }
            CLOCK_TIME::alarm bb;
            bb.name = new_courseName;
            bb.hour = new_startPeriod + 7;
            bb.week = day - 1;
            bb.place_link = new_classroom;
            for (int i = new_startWeek; i <= new_endWeek; i++)
            {
                bb.weekNum = i;
                this->logtxt->clk_time->alarm_list.push_back(bb);
            }
            bb.name = new_courseName + "考试";
            bb.weekNum = new_examTime;
            bb.week = new_examDay - 1;
            bb.hour = new_examStartPeriod + 7;
            bb.place_link = new_examRoom;
            this->logtxt->clk_time->alarm_list.push_back(bb);
        }

        dialog.close();
    });

    dialog.exec();
}
