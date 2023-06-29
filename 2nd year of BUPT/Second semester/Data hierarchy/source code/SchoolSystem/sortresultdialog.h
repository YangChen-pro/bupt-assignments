#ifndef SORTRESULTDIALOG_H
#define SORTRESULTDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include "mymap.h"
#include "Activity.h" // 引入 Activity 类的定义
#include <unordered_map> // 引入 unordered_map
#include "log.h"

// 为了方便起见，我们可以将 std::unordered_map<std::string, std::vector<std::string>> 定义为一个类型别名
using ActivityMap = std::unordered_map<std::string, std::vector<std::string>>;

class SortResultDialog : public QDialog
{
    Q_OBJECT

public:
    // 构造函数，接收活动映射和活动向量作为参数
    SortResultDialog(logTXT*logtxt,const ActivityMap &activityMap, const std::vector<Activity> &activities, QWidget *parent = nullptr);
    ~SortResultDialog();
    logTXT *logtxt;
private:
    // 指向表格的指针
    QTableWidget *resultTable;

    // 用于存储活动映射和活动向量的引用
    const ActivityMap &activityMap;
    std::vector<Activity> activities;

    // 用于设置表格内容的函数
    void setupTable();

private slots:
    // 当下拉框选项改变时触发的槽函数
    void onSortOptionChanged(int index);
};

#endif // SORTRESULTDIALOG_H
