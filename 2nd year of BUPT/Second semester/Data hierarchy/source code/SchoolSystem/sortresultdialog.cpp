#include "SortResultDialog.h"
#include <QComboBox>
#include <QVBoxLayout>
#include <QHeaderView>
#include <algorithm>
extern MyMap<string, int> day_to_int;
extern MyMap<int, string> int_to_day;
SortResultDialog::SortResultDialog(logTXT*logtxt,const ActivityMap &activityMap, const std::vector<Activity> &activities, QWidget *parent)
    : QDialog(parent), activityMap(activityMap), activities(activities)
{
    this->logtxt = logtxt;
    resultTable = new QTableWidget(this);

    // 创建一个下拉框（QComboBox）并添加选项
    QComboBox *sortComboBox = new QComboBox(this);
    sortComboBox->addItem("按时间排序");
    sortComboBox->addItem("按类型排序");

    // 连接下拉框的信号槽
    connect(sortComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SortResultDialog::onSortOptionChanged);

    // 将下拉框添加到布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(sortComboBox);
    layout->addWidget(resultTable);

    // 初始化表格
    onSortOptionChanged(0); // 默认按时间排序
}

void SortResultDialog::onSortOptionChanged(int index)
{
    // 根据下拉框的选项对活动进行排序
    if (index == 0) {
        // 按时间排序
        this->logtxt->log("用户按时间对课外活动排序 ");
        std::sort(activities.begin(), activities.end(),
                  [](const Activity &a, const Activity &b) {
                      if (a.start_week != b.start_week)
                      {
                          return a.start_week < b.start_week;
                      }
                      if (a.day != b.day)
                      {
                          return a.day < b.day;
                      }
                      return a.start_period < b.start_period;
                  });
    } else {
        // 按类型排序
        this->logtxt->log("用户按类型对课外活动排序 ");
        std::sort(activities.begin(), activities.end(),
                  [](const Activity &a, const Activity &b) {
                      if (a.activity_type != b.activity_type) {
                          return a.activity_type < b.activity_type;
                      }
                      else
                      {     if (a.start_week != b.start_week)
                          {
                              return a.start_week < b.start_week;
                          }
                          if (a.day != b.day)
                          {
                              return a.day < b.day;
                          }
                          return a.start_period < b.start_period;
                      }
                  });
    }

    // 更新表格
    setupTable();
}
SortResultDialog::~SortResultDialog()
{
    this->logtxt->clk_time->alarm_list.~vector();
    this->logtxt->clk_time->now_alarm_list.~vector();
}

void SortResultDialog::setupTable()
{
    resultTable->setRowCount(activities.size());
    resultTable->setColumnCount(4);

    QStringList headerLabels = {"活动名称", "活动类型", "开始时间", "结束时间"};
    resultTable->setHorizontalHeaderLabels(headerLabels);

    resultTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    resultTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    resultTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    resultTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    for (size_t i = 0; i < activities.size(); ++i) {
        const Activity &activity = activities[i];

        QTableWidgetItem *activityNameItem = new QTableWidgetItem(QString::fromStdString(activity.name));
        resultTable->setItem(i, 0, activityNameItem);

        QString activityType = activity.activity_type == 1 ? "个人活动" : "集体活动";
        QTableWidgetItem *activityTypeItem = new QTableWidgetItem(activityType);
        resultTable->setItem(i, 1, activityTypeItem);

        QString startTime = QString("第%1周 周%2 %3:00").arg(activity.start_week).arg(int_to_day[activity.day].c_str()).arg(activity.start_period + 5);
        QTableWidgetItem *startTimeItem = new QTableWidgetItem(startTime);
        resultTable->setItem(i, 2, startTimeItem);

        QString endTime = QString("第%1周 周%2 %3:00").arg(activity.end_week).arg(int_to_day[activity.day].c_str()).arg(activity.end_period + 5);
        QTableWidgetItem *endTimeItem = new QTableWidgetItem(endTime);
        resultTable->setItem(i, 3, endTimeItem);
    }
}
