#include "loginwidget.h"

LoginWidget::LoginWidget(Client *client, QWidget *parent) :
    QWidget(parent),
    m_title("登录注册"),
    m_loginButton("登录"),
    m_registerButton("注册"),
    m_client(client)
{
    m_layout.addWidget(&m_title);
    m_userIdInput.setPlaceholderText("用户名");
    m_layout.addWidget(&m_userIdInput);
    m_passwordInput.setPlaceholderText("密码");
    m_passwordInput.setEchoMode(QLineEdit::Password);
    m_layout.addWidget(&m_passwordInput);
    m_layout.addWidget(&m_loginButton);
    m_layout.addWidget(&m_registerButton);
    setLayout(&m_layout);

    connect(&m_loginButton, &QPushButton::clicked, this, &LoginWidget::onLoginButtonClicked);
    connect(&m_registerButton, &QPushButton::clicked, this, &LoginWidget::onRegisterButtonClicked);
    // client的信号和loginWidget的槽函数连接
    connect(m_client, &Client::loginSucceeded, this, &LoginWidget::onLoginSuccess);
    connect(m_client, &Client::loginFailed, this, &LoginWidget::onLoginFail);
    connect(m_client, &Client::registerSucceeded, this, &LoginWidget::onRegisterSuccess);
    connect(m_client, &Client::registerFailed, this, &LoginWidget::onRegisterFail);
}

LoginWidget::~LoginWidget()
{
}

void LoginWidget::InputIsEmpty(){
    QMessageBox::warning(this, "警告", "用户名和密码不能为空");
}

void LoginWidget::onLoginButtonClicked()
{
    if (m_userIdInput.text() == "" || m_passwordInput.text() == "")
    {
        InputIsEmpty();
        return;
    }
    m_client->sendLoginRequest(m_userIdInput.text(), m_passwordInput.text());
    // 登录成功与否的处理将在 Client 类中的 handleServerResponse 函数中进行
}

void LoginWidget::onRegisterButtonClicked()
{
    if (m_userIdInput.text() == "" || m_passwordInput.text() == "")
    {
        InputIsEmpty();
        return;
    }
    m_client->sendRegisterRequest(m_userIdInput.text(), m_passwordInput.text());
    // 注册成功与否的处理将在 Client 类中的 handleServerResponse 函数中进行
}


void LoginWidget::onLoginSuccess(){
    QMessageBox::information(this, tr("登录成功"), tr("您已成功登录"));
    emit loginSuccess();
}

void LoginWidget::onLoginFail(const QString &message){
    QMessageBox::warning(this, "提示", message + ", 请检查账号和密码是否正确");
}

void LoginWidget::onRegisterSuccess(){
    QMessageBox::information(this, tr("注册成功"), tr("您已成功注册一个新账号"));
}

void LoginWidget::onRegisterFail(const QString &message){
    QMessageBox::warning(this, "提示", message + ", 用户名已存在");
}
