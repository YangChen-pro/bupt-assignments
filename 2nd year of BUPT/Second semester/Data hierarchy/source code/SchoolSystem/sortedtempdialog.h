#ifndef SORTEDTEMPDIALOG_H
#define SORTEDTEMPDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include "mymap.h" // 包含MyMap的头文件
#include <string>
#include <vector>
#include "temporarything.h"
#include "log.h"

using namespace std;

class SortedTempDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SortedTempDialog(logTXT *log, const std::vector<TemporaryThing> &sortedThings, QWidget *parent = nullptr);
    ~SortedTempDialog();
    logTXT *logtxt;
private:
    QTableWidget *tempTable;

    // 将表格设置为sortedThings参数中的内容
    void setupTable(const std::vector<TemporaryThing> &sortedThings);
};

#endif // SORTEDTEMPDIALOG_H
