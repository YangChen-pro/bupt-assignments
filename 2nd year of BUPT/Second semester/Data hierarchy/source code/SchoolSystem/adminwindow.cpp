#include "adminwindow.h"
#include "schedulewindow.h"
#include <fstream>
#include <QStringList>
#include <QMessageBox>
#include <QFile>
#include <QByteArray>

AdminWindow::AdminWindow(logTXT*logtxt,QWidget *parent) : QWidget(parent)
{
    this->logtxt = logtxt;
    adminLabel = new QLabel("管理员", this);
    menuListWidget = new QListWidget(this);
    menuListWidget->setStyleSheet("QListWidget {background-color: #EDEDED; border: 1px solid #CCCCCC;} QListWidget::item:selected {background-color: #5F87D6; color: white;} QListWidget::item:hover {background-color: #7198E8; color: white;}");
    menuListWidget->addItem("添加学生");
    menuListWidget->addItem("显示所有学生");
    menuListWidget->addItem("管理课程安排");
    stackedWidget = new QStackedWidget(this);

    // 创建添加学生界面
    addStudentWidget = new QWidget();
    resize(1000, 400);

    QLabel *idLabel = new QLabel("学号：", addStudentWidget);
    QLabel *passwordLabel = new QLabel("密码：", addStudentWidget);
    QLabel *nameLabel = new QLabel("姓名：", addStudentWidget);
    idLineEdit = new QLineEdit(addStudentWidget);
    passwordLineEdit = new QLineEdit(addStudentWidget);
    nameLineEdit = new QLineEdit(addStudentWidget);
    passwordLineEdit->setEchoMode(QLineEdit::Password); // 设置密码输入框的显示模式为密码模式
    QPushButton *addStudentBtn = new QPushButton("立即添加", addStudentWidget);

    // 使用 QFormLayout 替换 QVBoxLayout
    QFormLayout *addStudentLayout = new QFormLayout(addStudentWidget);

    // 添加表单元素
    addStudentLayout->addRow(idLabel, idLineEdit);
    addStudentLayout->addRow(passwordLabel, passwordLineEdit);
    addStudentLayout->addRow(nameLabel, nameLineEdit);

    // 设置一些间距和对齐方式
    addStudentLayout->setContentsMargins(50, 50, 50, 50); // 设置左、上、右、下边距
    addStudentLayout->setSpacing(20); // 设置表单元素之间的间距

    QVBoxLayout *addStudentButtonLayout = new QVBoxLayout();
    addStudentButtonLayout->addWidget(addStudentBtn);
    addStudentButtonLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    addStudentLayout->addRow(addStudentButtonLayout); // 将按钮添加到布局中

    connect(addStudentBtn, &QPushButton::clicked, this, &AdminWindow::addStudent);
    stackedWidget->addWidget(addStudentWidget);

    // 创建显示所有学生界面
    QWidget *showAllStudentsWidget = new QWidget();
    QHBoxLayout *showAllStudentsLayout = new QHBoxLayout(showAllStudentsWidget);
    studentTableWidget = new QTableWidget(showAllStudentsWidget);
    showAllStudentsLayout->addWidget(studentTableWidget);
    stackedWidget->addWidget(showAllStudentsWidget);
    leftLayout = new QVBoxLayout();
    leftLayout->addWidget(adminLabel);
    leftLayout->addWidget(menuListWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(leftLayout);
    mainLayout->addWidget(stackedWidget);

    // 创建 ScheduleWindow 实例并将其添加到 stackedWidget
    ScheduleWindow *scheduleWindow = new ScheduleWindow(this->logtxt,"courses.txt");
    stackedWidget->addWidget(scheduleWindow);

    connect(menuListWidget, &QListWidget::currentRowChanged, stackedWidget, &QStackedWidget::setCurrentIndex);
    connect(menuListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(menuItemChanged(int)));
    connect(this, &AdminWindow::showAllStudentsSignal, this, &AdminWindow::showAllStudents);

}

AdminWindow::~AdminWindow()
{
    this->logtxt->clk_time->alarm_list.~vector();
    this->logtxt->clk_time->now_alarm_list.~vector();
}

void AdminWindow::menuItemChanged(int index)
{
    if (index == 1) { // 当选择“显示所有学生”菜单时
        emit showAllStudentsSignal(); // 发射 showAllStudentsSignal 信号
    }
}

void AdminWindow::addStudent()
{
    std::ofstream ofs("UserAccount.txt", std::ios::app);
    if (!ofs.is_open()) {
        QMessageBox::critical(this, "错误", "无法打开文件");
        return;
    }

    QString id = idLineEdit->text();
    QString password = passwordLineEdit->text();
    QString name = nameLineEdit->text();
    if (id == "" || password == "" || name == "")
        QMessageBox::information(this, "失败", "学号，密码，姓名不能为空");
    else
    {
        ofs << id.toStdString() << " " << password.toStdString() << " " << name.toStdString() << std::endl;
        idLineEdit->clear();
        passwordLineEdit->clear();
        nameLineEdit->clear();
        QMessageBox::information(this, "提示", "添加成功");
    }
}

void AdminWindow::showAllStudents()
{
    qDebug() << "showAllStudents() called";
    // 清空表格中的所有数据
    studentTableWidget->setRowCount(0);
    studentTableWidget->setColumnCount(4);
    studentTableWidget->setHorizontalHeaderLabels(QStringList() << "学号" << "密码" << "姓名" << "操作");
    QFile file("UserAccount.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开文件");
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(" ");
        if (fields.size() != 3) {
            // 如果数据格式不正确，跳过该行数据
            qDebug() << "格式不对";
            continue;
        }
        int row = studentTableWidget->rowCount();
        qDebug() << fields.at(0) << fields.at(1) << fields.at(2);
        studentTableWidget->insertRow(row);
        studentTableWidget->setItem(row, 0, new QTableWidgetItem(fields.at(0)));
        studentTableWidget->setItem(row, 1, new QTableWidgetItem(fields.at(1)));
        studentTableWidget->setItem(row, 2, new QTableWidgetItem(fields.at(2)));

        // 添加操作按钮
        QWidget *operationWidget = new QWidget();
        QHBoxLayout *operationLayout = new QHBoxLayout(operationWidget);
        operationLayout->setAlignment(Qt::AlignCenter);
        QPushButton *modifyBtn = new QPushButton("修改");
        QPushButton *deleteBtn = new QPushButton("删除");
        operationLayout->addWidget(modifyBtn);
        operationLayout->addWidget(deleteBtn);

        studentTableWidget->setCellWidget(row, 3, operationWidget);
        studentTableWidget->resizeColumnsToContents();
        studentTableWidget->resizeRowsToContents();
        // 连接槽函数
        connect(modifyBtn, &QPushButton::clicked, this, &AdminWindow::modifyStudent);
        connect(deleteBtn, &QPushButton::clicked, this, &AdminWindow::deleteStudent);
    }

    file.close();
}

void AdminWindow::deleteStudent()
{
    // 获取触发槽函数的按钮
    QPushButton *senderButton = qobject_cast<QPushButton *>(sender());
    if (!senderButton) return;
    // 获取按钮所在的行
    int row = -1;
    for (int i = 0; i < studentTableWidget->rowCount(); ++i) {
        if (studentTableWidget->cellWidget(i, 3) == senderButton->parent()) {
            row = i;
            break;
        }
    }

    if (row == -1) return;

    // 获取要删除的学生学号
    QString studentId = studentTableWidget->item(row, 0)->text();

    // 删除表格中的行
    studentTableWidget->removeRow(row);

    // 从文件中删除学生信息
    QFile inputFile("UserAccount.txt");
    QFile outputFile("UserAccount_temp.txt");
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text) || !outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开文件");
        return;
    }

    QTextStream in(&inputFile);
    QTextStream out(&outputFile);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.startsWith(studentId)) {
            out << line << "\n";
        }
    }

    inputFile.close();
    outputFile.close();

    if (QFile::remove("UserAccount.txt")) {
        if (!QFile::rename("UserAccount_temp.txt", "UserAccount.txt")) {
            QMessageBox::critical(this, "错误", "无法重命名文件");
        }
    } else {
        QMessageBox::critical(this, "错误", "无法删除文件");
    }
}

void AdminWindow::modifyStudent()
{
    // 获取触发槽函数的按钮
    QPushButton *senderButton = qobject_cast<QPushButton *>(sender());
    if (!senderButton) return;

    // 获取按钮所在的行
    int row = -1;
    for (int i = 0; i < studentTableWidget->rowCount(); ++i) {
        if (studentTableWidget->cellWidget(i, 3) == senderButton->parent()) {
            row = i;
            break;
        }
    }

    if (row == -1) return;

    // 获取当前行的学生信息
    QString studentId = studentTableWidget->item(row, 0)->text();
    QString studentPassword = studentTableWidget->item(row, 1)->text();
    QString studentName = studentTableWidget->item(row, 2)->text();

    // 弹出一个对话框，让用户输入新的信息
    QDialog *modifyDialog = new QDialog(this);
    modifyDialog->setWindowTitle("修改学生信息");
    QFormLayout *formLayout = new QFormLayout(modifyDialog);

    QLineEdit *idLineEdit = new QLineEdit(studentId, modifyDialog);
    QLineEdit *passwordLineEdit = new QLineEdit(studentPassword, modifyDialog);
    QLineEdit *nameLineEdit = new QLineEdit(studentName, modifyDialog);
    passwordLineEdit->setEchoMode(QLineEdit::Password);

    formLayout->addRow("学号：", idLineEdit);
    formLayout->addRow("密码：", passwordLineEdit);
    formLayout->addRow("姓名：", nameLineEdit);

    QPushButton *confirmBtn = new QPushButton("确认修改", modifyDialog);
    formLayout->addWidget(confirmBtn);

    connect(confirmBtn, &QPushButton::clicked, modifyDialog, &QDialog::accept);

    if (modifyDialog->exec() == QDialog::Accepted) {
        // 判断输入内容是否为空
        if (idLineEdit->text().isEmpty() || passwordLineEdit->text().isEmpty() || nameLineEdit->text().isEmpty()) {
            QMessageBox::warning(this, "警告", "输入内容不能为空");
            return;
        }

        // 更新表格中的数据
        studentTableWidget->item(row, 0)->setText(idLineEdit->text());
        studentTableWidget->item(row, 1)->setText(passwordLineEdit->text());
        studentTableWidget->item(row, 2)->setText(nameLineEdit->text());

        // 更新文件中的数据
        QFile inputFile("UserAccount.txt");
        QFile outputFile("UserAccount_temp.txt");
        if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text) || !outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "错误", "无法打开文件");
            return;
        }

        QTextStream in(&inputFile);
        QTextStream out(&outputFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(" ");
            if (fields.size() != 3) {
                continue;
            }

            if (fields.at(0) == studentId) {
                out << idLineEdit->text() << " " << passwordLineEdit->text() << " " << nameLineEdit->text() << "\n";
            } else {
                out << line << "\n";
            }
        }

        inputFile.close();
        outputFile.close();

        if (QFile::remove("UserAccount.txt")) {
            if (!QFile::rename("UserAccount_temp.txt", "UserAccount.txt")) {
                QMessageBox::critical(this, "错误", "无法重命名文件");
            }
        } else {
            QMessageBox::critical(this, "错误", "无法删除文件");
        }
    }

    delete modifyDialog;
}
