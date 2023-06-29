#include "loginwindow.h"

LoginWindow::LoginWindow(logTXT *log, QWidget *parent) : QWidget(parent)
{
    this->logtxt = log;
    usernameLabel = new QLabel("输入账号:", this);
    passwordLabel = new QLabel("输入密码:", this);

    usernameLineEdit = new QLineEdit(this);
    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    loginButton = new QPushButton("登录", this);
    // 下面这行为登录按钮设置样式表，你可以根据需求修改样式
    loginButton->setStyleSheet("QPushButton {background-color: #5F87D6; color: white; border-radius: 5px; font-weight: bold; font-size: 16px; padding: 5px;} QPushButton:hover {background-color: #7198E8;}");

    usernameLayout = new QHBoxLayout();
    usernameLayout->addWidget(usernameLabel);
    usernameLayout->addWidget(usernameLineEdit);
    passwordLayout = new QHBoxLayout();
    passwordLayout->addWidget(passwordLabel);
    passwordLayout->addWidget(passwordLineEdit);

    buttonLayout = new QVBoxLayout(); // 新建按钮布局
    buttonLayout->addStretch(); // 添加弹簧
    buttonLayout->addWidget(loginButton); // 将按钮添加到按钮布局中
    buttonLayout->addStretch(); // 添加弹簧

    mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(usernameLayout);
    mainLayout->addLayout(passwordLayout);
    mainLayout->addLayout(buttonLayout); // 使用按钮布局替换直接添加按钮

    adminWindow = new AdminWindow(log);
    studentWindow = nullptr; // 初始化 studentWindow 为 nullptr

    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginButtonClicked);
}

LoginWindow::~LoginWindow()
{
    delete adminWindow;
    delete studentWindow;
//    this->logtxt->clk_time->alarm_list.~vector();
//    this->logtxt->clk_time->now_alarm_list.~vector();
}

void LoginWindow::onLoginButtonClicked()
{
    QString username = usernameLineEdit->text();
    QString password = passwordLineEdit->text();

    this->logtxt->log("登录"+username.toStdString()+" "+password.toStdString());
    authenticate(username, password);

}

void LoginWindow::authenticate(const QString &username, const QString &password)
{
    bool isAdmin = false;
    bool isUser = false;

    QFile adminFile("AdminAccount.txt");
    if (adminFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&adminFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList adminInfo = line.split(" ");
            if (adminInfo.size() >= 2 &&
                adminInfo[0] == username &&
                adminInfo[1] == password) {
                isAdmin = true;
                break;
            }
        }
        adminFile.close();
    }

    QFile userFile("UserAccount.txt");
    if (!isAdmin && userFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&userFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList userInfo = line.split(" ");
            if (userInfo.size() >= 3 &&
                userInfo[0] == username &&
                userInfo[1] == password) {
                isUser = true;
                break;
            }
        }
        userFile.close();
    }

    if (isAdmin) {
        adminWindow->show();

        this->logtxt->log("登陆成功，为管理员");

        this->hide();
    } else if (isUser) {
        studentWindow = new StudentWindow(this->logtxt,username); // 创建一个新的 StudentWindow 对象，并传递当前登录学生的用户名
        studentWindow->show();

        this->logtxt->log("登陆成功，为用户");
        this->hide();
    } else {
        QMessageBox::warning(this, "登录失败", "用户名或密码错误");
        this->logtxt->log("登陆失败");
    }
}
