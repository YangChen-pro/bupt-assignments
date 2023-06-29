#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include "client.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(Client *client, QWidget *parent = nullptr);
    ~LoginWidget();
    void InputIsEmpty();

signals:
    void loginSuccess();

private slots:
    void onLoginButtonClicked();
    void onRegisterButtonClicked();
    void onLoginSuccess();
    void onLoginFail(const QString &message);
    void onRegisterSuccess();
    void onRegisterFail(const QString &message);

private:
    QVBoxLayout m_layout;
    QLabel m_title;
    QLineEdit m_userIdInput;
    QLineEdit m_passwordInput;
    QPushButton m_loginButton;
    QPushButton m_registerButton;
    Client *m_client;
};

#endif // LOGINWIDGET_H
