#include "activitywindow.h"

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
extern void log(std::string str,CLOCK_TIME *clk);
//extern CLOCK_TIME *clk;
ActivityWindow::ActivityWindow(logTXT*logtxt,const string personalFile,QWidget *parent)
    : QWidget(parent)
{

    //this->weekNum = logtxt->clk_time->getWeekNum();


    this->logtxt = logtxt;
    // 添加周数标签
    weekLabel = new QLabel(this);
    //mainLayout->addWidget(weekLabel);
    weekLabel->setText("第1周");

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        this->weekNum = this->logtxt->clk_time->getWeekNum() + 1;
        weekLabel->setText(QString::fromStdString("第"+to_string(this->weekNum)+"周"));
    });
    timer->start(2000);
    activity_schedule = new ActivitySchedule(personalFile, "group_activities.txt");
    PersonalFile = personalFile; // 记录个人活动文件的名字
    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(weekLabel);




    searchLayout = new QHBoxLayout();
    searchInput = new QLineEdit(this);
    btnFindActivity = new QPushButton("查找活动", this);
    searchLayout->addWidget(searchInput);
    searchLayout->addWidget(btnFindActivity);

    findResultDisplay = new QTextEdit(this);
    findResultDisplay->setReadOnly(true);
    mainLayout->addWidget(findResultDisplay);

    addLayout = new QHBoxLayout();
    btnAddActivity = new QPushButton("添加活动", this);
    addLayout->addWidget(btnAddActivity);

    btnSortActivities = new QPushButton("排序活动", this);
    addLayout->addWidget(btnSortActivities);

    createActivityTable();
    updateActivityTable();

    mainLayout->addLayout(searchLayout);
    mainLayout->addWidget(activityDisplay);
    mainLayout->addLayout(addLayout);

    connect(btnFindActivity, &QPushButton::clicked, this, &ActivityWindow::onFindActivityClicked);
    connect(btnAddActivity, &QPushButton::clicked, this, &ActivityWindow::onAddActivityClicked);
    connect(btnSortActivities, &QPushButton::clicked, this, &ActivityWindow::onSortActivitiesClicked);
}

ActivityWindow::~ActivityWindow()
{
    delete activity_schedule;
    this->logtxt->clk_time->alarm_list.~vector();
    this->logtxt->clk_time->now_alarm_list.~vector();
}

void ActivityWindow::createActivityTable()
{
    // 创建表格
    activityDisplay = new QTableWidget(16, 7, this);
    activityDisplay->setEditTriggers(QAbstractItemView::NoEditTriggers);
    activityDisplay->setSelectionMode(QAbstractItemView::SingleSelection);

    // 设置表格行标题
    QStringList rowLabels;
    for (int i = 6; i < 22; ++i)
    {
        rowLabels << QString("%1:00~%2:00").arg(QString::number(i), 2, '0').arg(QString::number(i+1), 2, '0');
    }
    activityDisplay->setVerticalHeaderLabels(rowLabels);

    // 设置表格列标题
    QStringList columnLabels;
    columnLabels << "周一" << "周二" << "周三" << "周四" << "周五" << "周六" << "周日";
    activityDisplay->setHorizontalHeaderLabels(columnLabels);

    // 连接单元格点击信号
    connect(activityDisplay, &QTableWidget::cellClicked, this, &ActivityWindow::onActivityCellClicked);
}

void ActivityWindow::updateActivityTable()
{
    // 更新周数标签
    //weekLabel->setText(tr("第 %1 周").arg(this->weekNum));

    // 更新表格内容
    for (int row = 0; row < 16; ++row)
    {
        for (int col = 0; col < 7; ++col)
        {
            QString text = QString::fromStdString(activity_schedule->getActivity(weekNum,col,row));
            QTableWidgetItem *item = new QTableWidgetItem(text);
            activityDisplay->setItem(row, col, item);
        }
    }
    activityDisplay->resizeColumnsToContents();
    activityDisplay->resizeRowsToContents();
}


void ActivityWindow::onFindActivityClicked()
{
    QString activityName = searchInput->text();
    QString findResult = activity_schedule->find_activity(activityName.toStdString());
    this->logtxt->log("用户查找活动 "+activityName.toStdString());

    findResultDisplay->setText(findResult);
}

void ActivityWindow::onSortActivitiesClicked() {
    // 实例化 SortResultDialog 并显示
    this->logtxt->log("用户对课外活动排序");
    SortResultDialog sortResultDialog(this->logtxt,activity_schedule->ActivityMap, activity_schedule->activities, this);

    sortResultDialog.exec();
}

void ActivityWindow::onAddActivityClicked()
{
    // 创建一个QDialog窗口，命名为addDialog，作为本窗口的子窗口
    QDialog addDialog(this);
    addDialog.setWindowTitle(tr("添加活动"));
    QFormLayout formLayout(&addDialog);
    this->logtxt->log("用户添加课外活动");

    // 创建用于输入活动信息的控件（QLineEdit和QSpinBox）
    QLineEdit edtActivity(tr("活动名称"), &addDialog);
    QLineEdit edtClassroom(tr("活动地点"), &addDialog);
    QSpinBox spbType(&addDialog);
    QSpinBox spbStartWeek(&addDialog);
    QSpinBox spbEndWeek(&addDialog);
    QSpinBox spbDay(&addDialog);
    QSpinBox spbStartPeriod(&addDialog);
    QSpinBox spbEndPeriod(&addDialog);


    spbStartWeek.setRange(1, 20);
    spbEndWeek.setRange(1, 20);
    spbType.setRange(1, 2);
    spbDay.setRange(1, 7);
    spbStartPeriod.setRange(1, 16);
    spbEndPeriod.setRange(1, 16);


    // 将控件添加到formLayout布局中，并设置标签文本
    formLayout.addRow(tr("活动名称："), &edtActivity);
    formLayout.addRow(tr("活动类型：1个人活动,2集体活动"), &spbType);
    formLayout.addRow(tr("活动地点："), &edtClassroom);
    formLayout.addRow(tr("开始周："), &spbStartWeek);
    formLayout.addRow(tr("结束周："), &spbEndWeek);
    formLayout.addRow(tr("星期几："), &spbDay);
    formLayout.addRow(tr("开始时间（节）："), &spbStartPeriod);
    formLayout.addRow(tr("结束时间（节）："), &spbEndPeriod);

    // 添加确认和取消按钮
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &addDialog);
    formLayout.addRow(&buttonBox);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &addDialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &addDialog, &QDialog::reject);

    // 显示对话框并等待用户操作
    int result = addDialog.exec();
    if (result == QDialog::Accepted) {
        // 用户点击了确认按钮，获取输入的活动信息
        std::string name = edtActivity.text().toStdString();
        std::string classroom = edtClassroom.text().toStdString();
        int type = spbType.value();
        int startWeek = spbStartWeek.value();
        int endWeek = spbEndWeek.value();
        int day = spbDay.value();
        int startPeriod = spbStartPeriod.value();
        int endPeriod = spbEndPeriod.value();

        if (activity_schedule->checkActivityConflict(type, startWeek, endWeek, day, startPeriod, endPeriod))
        {
            if (startWeek == endWeek) {
                QVector<std::pair<int, int>> availableTimes;
                QMap<int, QVector<std::pair<int, int>>> conflictCounts;
                Person p;
                for (int period = 0; period <= 15; ++period)
                {
                    int conflictCount = p.things[startWeek][day][period];
                    if (conflictCount == 0)
                    {
                        availableTimes.push_back(std::make_pair(period, period));
                    }
                    else
                    {
                        if (!conflictCounts.contains(conflictCount)) {
                            conflictCounts[conflictCount] = QVector<std::pair<int, int>>();
                        }
                        conflictCounts[conflictCount].push_back(std::make_pair(period, period));
                    }
                }

                if (!availableTimes.isEmpty()) {
                    // 有完美解决方案，选3种
                    int numSuggestions = std::min(3, (int)availableTimes.size());
                    QString suggestedTimes;
                    for (int i = 0; i < numSuggestions; ++i) {
                        std::pair<int, int> time = availableTimes[i];
                        suggestedTimes += QString("时间段 %1: %2 - %3\n").arg(i + 1).arg(time.first).arg(time.second);
                    }
                    this->logtxt->log("活动添加失败");
                    QMessageBox::information(this, tr("可用时间建议"), tr("以下是3个可用的时间段建议：\n%1").arg(suggestedTimes));
                }
                else {
                    if (type == 1) {
                        // 个人活动，提示失败
                        this->logtxt->log("活动添加失败");
                        QMessageBox::warning(this, tr("添加活动失败"), tr("没有可用的时间段来安排这个个人活动。"));
                    }
                    else
                    {
                        // 集体活动，寻找冲突人数最少
                        int minConflicts = std::numeric_limits<int>::max(); // int最大值
                        for (auto it = conflictCounts.constBegin(); it != conflictCounts.constEnd(); ++it) {
                            if (it.key() < minConflicts) {
                                minConflicts = it.key();
                            }
                        }

                        if (minConflicts != std::numeric_limits<int>::max()) {
                            // 给出冲突人数最少的3个时间段
                            QVector<std::pair<int, int>> timesWithMinConflicts = conflictCounts[minConflicts];
                            int numSuggestions = std::min(3, (int)timesWithMinConflicts.size());
                            QString suggestedTimes;
                            for (int i = 0; i < numSuggestions; ++i) {
                                std::pair<int, int> time = timesWithMinConflicts[i];
                                suggestedTimes += QString("时间段 %1: %2 - %3 (冲突人数: %4)\n").arg(i + 1).arg(time.first).arg(time.second).arg(minConflicts);
                            }
                            this->logtxt->log("活动添加失败");
                            QMessageBox::information(this, tr("冲突人数最少的时间建议"), tr("以下是3个冲突人数最少的时间段建议：\n%1").arg(suggestedTimes));
                        }
                        else { // 理论上，除非所有活动的冲突人数为int_max，才会进入这里
                            this->logtxt->log("活动添加失败");
                            QMessageBox::warning(this, tr("添加活动失败"), tr("没有找到合适的时间来安排这个集体活动。"));
                        }
                    }
                }
            }
            else {
                // 周期性活动，建议换单次添加
                QMessageBox::warning(this, tr("添加活动失败"), tr("这段时间里有冲突，请尝试单次添加。"));
                this->logtxt->log("活动添加失败");
                return;
            }
        }
        else // 不冲突直接添加
        {
            // 调用Activity类的AddCourse函数添加活动
            Activity a(name, startWeek, endWeek, day - 1, startPeriod, endPeriod, classroom, type);
            this->logtxt->log("活动修改内容"+name+classroom+"类型："+to_string(type)+" "+to_string(startWeek)+"-"+to_string(endWeek)+"day"+to_string(day)+" "+to_string(startPeriod)+" "+to_string(endPeriod));
            activity_schedule->add_activity(a, PersonalFile);

            CLOCK_TIME::alarm bb;
            bb.name = name;
            bb.hour = startPeriod + 5;
            bb.week = day - 1;
            bb.place_link = classroom;
            for (int i = startWeek; i <= endWeek; i++)
            {
                bb.weekNum = i;
                this->logtxt->clk_time->alarm_list.push_back(bb);
            }
        }


        // 更新activityDisplay
        updateActivityTable();
    }
}

void ActivityWindow::onActivityCellClicked(int row, int col)
{
    QTableWidgetItem *item = activityDisplay->item(row, col);
    QString activityName = item->text();
    if (activityName.isEmpty()|| weekNum >= 16)
    {
        return;
    }

    QDialog dialog(this);
    QVBoxLayout layout(&dialog);

    QLabel label("你想做什么？", &dialog);
    this->logtxt->log("用户点击课外活动 第"+to_string(row+1)+"行 第"+to_string(col+1)+"列");
    layout.addWidget(&label);

    int startWeek = 1;
    int endWeek = 20;
    int day = col;
    int startPeriod = row + 1;
    // 寻找一次活动中的所有方格
    int endPeriod = row + 1;
    for (int i = row + 1; i < 22; i++) {
        QTableWidgetItem *nextItem = activityDisplay->item(i, col);
        if (nextItem->text() != activityName) {
            break;
        }
        endPeriod++;
    }

    //qDebug()<<activityName;
    QPushButton btnDelete("删除活动", &dialog);
    layout.addWidget(&btnDelete);
    connect(&btnDelete, &QPushButton::clicked, [&]() {
        this->logtxt->log("用户删除课外活动 第"+to_string(row+1)+"行 第"+to_string(col+1)+"列");
        activity_schedule->delete_activity(activityName.toStdString(), PersonalFile, startWeek, endWeek, day, startPeriod, endPeriod);
        updateActivityTable();
        dialog.close();

        for(int j=0;j<this->logtxt->clk_time->alarm_list.size();j++)
        {
            if(this->logtxt->clk_time->alarm_list[j].name==activityName.toStdString())
                this->logtxt->clk_time->alarm_list.erase(j+this->logtxt->clk_time->alarm_list.begin());
        }
    });

    QPushButton btnEdit("修改活动", &dialog);
    layout.addWidget(&btnEdit);
    connect(&btnEdit, &QPushButton::clicked, [&]() {
        this->logtxt->log("用户修改课外活动 第"+to_string(row+1)+"行 第"+to_string(col+1)+"列");
        QDialog editDialog(&dialog);
        QFormLayout formLayout(&editDialog);

        QLineEdit edtCourse(tr("活动名称"), &editDialog);
        QLineEdit edtClassroom(tr("活动地点"), &editDialog);
        QSpinBox spbStartWeek(&editDialog);
        QSpinBox spbType(&editDialog);
        QSpinBox spbEndWeek(&editDialog);
        QSpinBox spbDay(&editDialog);
        QSpinBox spbStartPeriod(&editDialog);
        QSpinBox spbEndPeriod(&editDialog);

        formLayout.addRow(tr("活动名称："), &edtCourse);
        formLayout.addRow(tr("活动类型：1个人活动,2集体活动"), &spbType);
        formLayout.addRow(tr("活动地点："), &edtClassroom);
        formLayout.addRow(tr("开始周："), &spbStartWeek);
        formLayout.addRow(tr("结束周："), &spbEndWeek);
        formLayout.addRow(tr("星期几："), &spbDay);
        formLayout.addRow(tr("开始时间（节）："), &spbStartPeriod);
        formLayout.addRow(tr("结束时间（节）："), &spbEndPeriod);

        spbStartWeek.setRange(1, 20);
        spbEndWeek.setRange(1, 20);
        spbType.setRange(1, 2);
        spbDay.setRange(1, 7);
        spbStartPeriod.setRange(1, 16);
        spbEndPeriod.setRange(1, 16);

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
            string new_activityName = edtCourse.text().toStdString();
            string new_classroom = edtClassroom.text().toStdString();
            int new_type = spbType.value();

            // 修改的实质是把旧的活动删除，把新的活动添加进来
            activity_schedule->delete_activity(activityName.toStdString(), PersonalFile, startWeek, endWeek, day, startPeriod, endPeriod);

            this->logtxt->log("活动修改内容"+new_activityName+new_classroom+"类型："+to_string(new_type)+" "+to_string(new_startWeek)+"-"+to_string(new_endWeek)+"day"+to_string(new_day)+" "+to_string(new_startPeriod)+" "+to_string(new_endPeriod));
            Activity a(new_activityName, new_startWeek, new_endWeek, new_day - 1, new_startPeriod, new_endPeriod, new_classroom, new_type);
            activity_schedule->add_activity(a, PersonalFile);
            updateActivityTable();

            for(int j=0;j<this->logtxt->clk_time->alarm_list.size();j++)
            {
                if(this->logtxt->clk_time->alarm_list[j].name==activityName.toStdString())
                    this->logtxt->clk_time->alarm_list.erase(j+this->logtxt->clk_time->alarm_list.begin());
            }

            CLOCK_TIME::alarm bb;
            bb.name = new_activityName;
            bb.hour = new_startPeriod + 5;
            bb.week = new_day - 1;
            bb.place_link = new_classroom;
            for (int i = new_startWeek; i <= new_endWeek; i++)
            {
                bb.weekNum = i;
                this->logtxt->clk_time->alarm_list.push_back(bb);
            }
        }

        dialog.close();
    });

    dialog.exec();
}
