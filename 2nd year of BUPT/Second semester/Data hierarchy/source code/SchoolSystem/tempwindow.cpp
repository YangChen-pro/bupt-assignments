#include "tempwindow.h"
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

TempWindow::TempWindow(logTXT*logtxt,const string personalFile, QWidget *parent)
    : QWidget(parent)
{

    this->logtxt = logtxt;
    Temp_schedule = new TemporarySchedule(personalFile);
    PersonalFile = personalFile; // 记录个人临时事务文件的名字
    mainLayout = new QVBoxLayout(this);



    weekLabel = new QLabel(this);

    weekLabel->setText("第1周");

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        this->weekNum = this->logtxt->clk_time->getWeekNum() + 1;
        weekLabel->setText(QString::fromStdString("第"+to_string(this->weekNum)+"周"));
    });
    // 添加周数标签
    timer->start();
    mainLayout->addWidget(weekLabel);
    //weekLabel->setText(QString::fromStdString(this->clk_time->weekTostring(this->clk_time->getWeekNum())));

    searchLayout = new QHBoxLayout();



    searchInput = new QLineEdit(this);
    btnFindTemp = new QPushButton("查找临时事务", this);
    searchLayout->addWidget(searchInput);
    searchLayout->addWidget(btnFindTemp);

    findResultDisplay = new QTextEdit(this);
    findResultDisplay->setReadOnly(true);
    mainLayout->addWidget(findResultDisplay);

    addLayout = new QHBoxLayout();
    btnAddTemp = new QPushButton("添加临时事务", this);
    addLayout->addWidget(btnAddTemp);

    btnSortTemp = new QPushButton("排序临时事务", this);
    addLayout->addWidget(btnSortTemp);

    createTempTable();
    updateTempTable();

    mainLayout->addLayout(searchLayout);
    mainLayout->addWidget(TempDisplay);
    mainLayout->addLayout(addLayout);

    connect(btnFindTemp, &QPushButton::clicked, this, &TempWindow::onFindTempClicked);
    connect(btnAddTemp, &QPushButton::clicked, this, &TempWindow::onAddTempClicked);
    connect(btnSortTemp, &QPushButton::clicked, this, &TempWindow::sortTempByTime);
}

TempWindow::~TempWindow()
{
    delete Temp_schedule;
    this->logtxt->clk_time->alarm_list.~vector();
    this->logtxt->clk_time->now_alarm_list.~vector();
}

void TempWindow::createTempTable()
{
    // 创建表格
    TempDisplay = new QTableWidget(16, 7, this);
    TempDisplay->setEditTriggers(QAbstractItemView::NoEditTriggers);
    TempDisplay->setSelectionMode(QAbstractItemView::SingleSelection);

    // 设置表格行标题
    QStringList rowLabels;
    for (int i = 6; i < 22; ++i)
    {
        rowLabels << QString("%1:00~%2:00").arg(QString::number(i), 2, '0').arg(QString::number(i+1), 2, '0');
    }
    TempDisplay->setVerticalHeaderLabels(rowLabels);

    // 设置表格列标题
    QStringList columnLabels;
    columnLabels << "周一" << "周二" << "周三" << "周四" << "周五" << "周六" << "周日";
    TempDisplay->setHorizontalHeaderLabels(columnLabels);

    // 连接单元格点击信号
    connect(TempDisplay, &QTableWidget::cellClicked, this, &TempWindow::onTempCellClicked);
}

void TempWindow::updateTempTable()
{
    // 更新周数标签
    //weekLabel->setText(tr("第 %1 周").arg(this->weekNum));

    // 更新表格内容
    for (int row = 0; row < 16; ++row)
    {
        for (int col = 0; col < 7; ++col)
        {
            QString text = QString::fromStdString(Temp_schedule->getTemp(weekNum,col,row+1));
            QTableWidgetItem *item = new QTableWidgetItem(text);
            TempDisplay->setItem(row, col, item);
        }
    }
    TempDisplay->resizeColumnsToContents();
    TempDisplay->resizeRowsToContents();
}


void TempWindow::onFindTempClicked()
{
    QString tempName = searchInput->text();
    QString findResult = Temp_schedule->find_thing(tempName.toStdString());
    this->logtxt->log("用户查找临时事务："+tempName.toStdString());
    findResultDisplay->setText(findResult);

}

void TempWindow::sortTempByTime()
{
    // 创建并显示排序后的临时事务对话框
    this->logtxt->log("用户对临时事务排序");
    SortedTempDialog *dialog = new SortedTempDialog(logtxt, Temp_schedule->things, this);
    dialog->exec();
}

void TempWindow::onAddTempClicked()
{
    // 创建一个QDialog窗口，命名为addDialog，作为本窗口的子窗口
    QDialog addDialog(this);
    addDialog.setWindowTitle(tr("添加临时事务"));
    QFormLayout formLayout(&addDialog);
    this->logtxt->log("用户添加临时事务");

    // 创建用于输入临时事务信息的控件（QLineEdit和QDateTimeEdit）
    QLineEdit edtName(tr("事务名称"), &addDialog);
    QLineEdit edtLocation(tr("事务地点"), &addDialog);
    QSpinBox edtWeek(&addDialog);
    QSpinBox edtDay(&addDialog);
    QSpinBox edtDateTime(&addDialog);
    QSpinBox edtDateMin(&addDialog);

    // 设置QSpinBox的范围
    edtWeek.setRange(1, 20);
    edtDay.setRange(1, 7);
    edtDateTime.setRange(1, 16);
    edtDateMin.setRange(1, 16);

    // 将控件添加到formLayout布局中，并设置标签文本
    formLayout.addRow(tr("事务名称："), &edtName);
    formLayout.addRow(tr("事务地点："), &edtLocation);
    formLayout.addRow(tr("第几周："), &edtWeek);
    formLayout.addRow(tr("星期几："), &edtDay);
    formLayout.addRow(tr("第几节开始："), &edtDateTime);
    formLayout.addRow(tr("开始分钟："), &edtDateMin);

    // 添加确认和取消按钮
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &addDialog);
    formLayout.addRow(&buttonBox);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &addDialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &addDialog, &QDialog::reject);

    // 显示对话框并等待用户操作
    int result = addDialog.exec();
    if (result == QDialog::Accepted) {
        // 用户点击了确认按钮，获取输入的临时事务信息

        std::string name = edtName.text().toStdString();
        std::string location = edtLocation.text().toStdString();
        int DateTime = edtDateTime.value();
        int DateMin = edtDateMin.value();
        int Week = edtWeek.value();
        int Day = edtDay.value();
        this->logtxt->log("添加内容："+name+" "+location+to_string(Week)+" "+to_string(Day)+" "+to_string(DateTime)+" "+to_string(DateMin));

//        if (Temp_schedule->checkTempScheduleConflict(Week, Day, DateTime))
//        {
//            QMessageBox::warning(this, tr("添加临时事物失败"), tr("该时间段内有其他临时事物"));
//            return;
//        }
        if (Temp_schedule->checkTempConflict(Week, Day, DateTime))
        {
            QMessageBox::warning(this, tr("添加临时事物失败"), tr("该时间段内有其他活动或课程"));
            return;
        }

        // 调用TemporaryThing类的AddTemp函数添加临时事务

        TemporaryThing a(name, Week, Day - 1, DateTime, DateMin, location);
        Temp_schedule->add_thing(a, PersonalFile);
        CLOCK_TIME::alarm bb;
        bb.name = name;
        bb.hour = DateTime + 5;
        bb.week = Day - 1;
        bb.place_link = location;
        bb.weekNum = Week;
        this->logtxt->clk_time->alarm_list.push_back(bb);
        // 更新TempDisplay
        updateTempTable();
    }
}

void TempWindow::onTempCellClicked(int row, int col)
{
    QTableWidgetItem *item = TempDisplay->item(row, col);
    this->logtxt->log("用户点击临时事务 第"+to_string(row+1)+"行 第"+to_string(col+1)+"列");
    QString tempName = item->text();
    if (tempName.isEmpty())
    {
        return;
    }
    // 可能有多个临时事务，按照"\n"分割
    QStringList qStringList = tempName.split("\n");
    vector<string> stringVec;
    for (const QString &qStr : qStringList) {
        stringVec.push_back(qStr.toStdString());
    }

    QDialog dialog(this);
    QVBoxLayout layout(&dialog);

    QLabel label("你想做什么？", &dialog);
    layout.addWidget(&label);

    QPushButton btnDelete("删除临时事务", &dialog);
    this->logtxt->log("用户删除临时事务 第"+to_string(row+1)+"行 第"+to_string(col+1)+"列");
    layout.addWidget(&btnDelete);
    connect(&btnDelete, &QPushButton::clicked, [&]() {
        for (size_t i = 0; i < stringVec.size(); ++i) {
            Temp_schedule->delete_thing(stringVec[i], PersonalFile);
        }

        for(int i=0;i<stringVec.size();i++)
        {
            for(int j=0;j<this->logtxt->clk_time->alarm_list.size();j++)
            {
                if(this->logtxt->clk_time->alarm_list[j].name==stringVec[i])
                    this->logtxt->clk_time->alarm_list.erase(j+this->logtxt->clk_time->alarm_list.begin());
            }
        }
        updateTempTable();
        dialog.close();
    });

    QPushButton btnEdit("修改临时事务", &dialog);
    this->logtxt->log("用户修改临时事务 第"+to_string(row+1)+"行 第"+to_string(col+1)+"列");
    layout.addWidget(&btnEdit);
    connect(&btnEdit, &QPushButton::clicked, [&]() {
        QDialog editDialog(&dialog);
        QFormLayout formLayout(&editDialog);

        QLineEdit edtName(tr("事务名称"), &editDialog);
        QLineEdit edtLocation(tr("事务地点"), &editDialog);
        QSpinBox edtDateTime(&editDialog);
        QSpinBox edtDateMin(&editDialog);
        QSpinBox edtWeek(&editDialog);
        QSpinBox edtDay(&editDialog);

        formLayout.addRow(tr("事务名称："), &edtName);
        formLayout.addRow(tr("事务地点："), &edtLocation);
        formLayout.addRow(tr("第几周："), &edtWeek);
        formLayout.addRow(tr("星期几："), &edtDay);
        formLayout.addRow(tr("第几节开始："), &edtDateTime);
        formLayout.addRow(tr("开始分钟："), &edtDateMin);

        // 设置QSpinBox的范围
        edtWeek.setRange(1, 20);
        edtDay.setRange(1, 7);
        edtDateTime.setRange(1, 16);
        edtDateMin.setRange(1, 16);

        QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &editDialog);
        formLayout.addRow(&buttonBox);
        connect(&buttonBox, &QDialogButtonBox::accepted, &editDialog, &QDialog::accept);
        connect(&buttonBox, &QDialogButtonBox::rejected, &editDialog, &QDialog::reject);

        if (editDialog.exec() == QDialog::Accepted)
        {
            std::string name = edtName.text().toStdString();
            std::string location = edtLocation.text().toStdString();
            int DateTime = edtDateTime.value();
            int DateMin = edtDateMin.value();
            int Week = edtWeek.value();
            int Day = edtDay.value();
            // 修改的实质是把旧的临时事务删除，把新的临时事务添加进来
            this->logtxt->log("添加内容："+name+" "+location+to_string(Week)+" "+to_string(Day)+" "+to_string(DateTime)+" "+to_string(DateMin));

            for (size_t i = 0; i < stringVec.size(); ++i) {
                Temp_schedule->delete_thing(stringVec[i], PersonalFile);
            }
            // 删除提醒表项
            for(int i=0;i<stringVec.size();i++)
            {
                for(int j=0;j<this->logtxt->clk_time->alarm_list.size();j++)
                {
                    if(this->logtxt->clk_time->alarm_list[j].name==stringVec[i])
                        this->logtxt->clk_time->alarm_list.erase(j+this->logtxt->clk_time->alarm_list.begin());
                }
            }
            TemporaryThing a(name, Week, Day - 1, DateTime, DateMin, location);
            Temp_schedule->add_thing(a, PersonalFile);

            CLOCK_TIME::alarm bb;
            bb.name = name;
            bb.hour = DateTime + 5;
            bb.week = Day - 1;
            bb.place_link = location;
            bb.weekNum = Week;
            this->logtxt->clk_time->alarm_list.push_back(bb);

            updateTempTable();
        }

        dialog.close();
    });

    dialog.exec();
}
