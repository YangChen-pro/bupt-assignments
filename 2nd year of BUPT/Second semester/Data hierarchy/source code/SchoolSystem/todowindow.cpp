#include "todowindow.h"

ToDoWindow::ToDoWindow(const QString &name, const QString &studentId, QWidget *parent) : QWidget(parent)
{
    nameLabel = new QLabel(name, this); // 显示学生姓名
    studentIdLabel = new QLabel(studentId, this); // 显示学生学号
    menuListWidget = new QListWidget(this);
    menuListWidget->addItem("课程表");
    menuListWidget->addItem("临时事物");
    menuListWidget->addItem("课外活动");
    stackedWidget = new QStackedWidget(this);

    // 在这里添加课程表、临时事物、课外活动的界面
    // ...

    topLayout = new QHBoxLayout;
    topLayout->addWidget(nameLabel);
    topLayout->addWidget(studentIdLabel);
    topLayout->addStretch();

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(menuListWidget);
    mainLayout->addWidget(stackedWidget);

    connect(menuListWidget, &QListWidget::currentRowChanged, stackedWidget, &QStackedWidget::setCurrentIndex);
}

ToDoWindow::~ToDoWindow()
{
}
