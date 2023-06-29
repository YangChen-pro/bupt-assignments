#include "sortedtempdialog.h"
#include <QComboBox>
#include <QVBoxLayout>
#include <QHeaderView>
#include <algorithm>
#include <string>
extern MyMap<string, int> day_to_int;
extern MyMap<int, string> int_to_day;

SortedTempDialog::SortedTempDialog(logTXT *log, const std::vector<TemporaryThing> &sortedThings, QWidget *parent)
    : QDialog(parent)
{
    logtxt = log;
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 创建一个下拉框（QComboBox）并添加选项
    QComboBox *sortComboBox = new QComboBox(this);
    sortComboBox->addItem("按类型排序");
    sortComboBox->addItem("按时间排序");

    // 连接下拉框的信号槽
    connect(sortComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, sortedThings](int index) {
        std::vector<TemporaryThing> localSortedThings = sortedThings;

        // 根据下拉框的选项对事物进行排序
        if (index == 0) {
            // 按名称排序
            logtxt->log("用户对临时事物按照类型排序");
            std::sort(localSortedThings.begin(), localSortedThings.end(),
                      [](const TemporaryThing &a, const TemporaryThing &b) {
                          return a.name < b.name;
                      });
        } else {
            // 按时间排序
            logtxt->log("用户对临时事物按照时间排序");
            std::sort(localSortedThings.begin(), localSortedThings.end(),
                      [](const TemporaryThing &a, const TemporaryThing &b) {
                          if (a.week != b.week) {
                              return a.week < b.week;
                          }
                          if (a.day != b.day) {
                              return a.day < b.day;
                          }
                          if (a.hour != b.hour) {
                              return a.hour < b.hour;
                          }
                          return a.minute < b.minute;
                      });
        }

        // 更新表格
        setupTable(localSortedThings);
    });

    // 将下拉框添加到布局
    layout->addWidget(sortComboBox);

    tempTable = new QTableWidget(sortedThings.size(), 3, this);
    tempTable->setHorizontalHeaderLabels({"事物名称", "事物时间", "事务地点"});

    tempTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tempTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tempTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    // 初始化表格
    sortComboBox->setCurrentIndex(0); // 默认按名称排序

    layout->addWidget(tempTable);
}

void SortedTempDialog::setupTable(const std::vector<TemporaryThing> &sortedThings)
{
    for (size_t i = 0; i < sortedThings.size(); ++i) {
        const TemporaryThing &thing = sortedThings[i];
        tempTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(thing.name)));

        QString timeText = QString("第%1周 周%2 %3点%4分")
                               .arg(thing.week)
                               .arg(int_to_day[thing.day].c_str())
                               .arg(thing.hour + 5)
                               .arg(thing.minute);
        tempTable->setItem(i, 1, new QTableWidgetItem(timeText));

        tempTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(thing.location)));
    }
}

SortedTempDialog::~SortedTempDialog()
{
}
