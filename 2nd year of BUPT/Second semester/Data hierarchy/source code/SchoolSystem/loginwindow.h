#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include "adminwindow.h"
#include "studentwindow.h"
#include "CLOCK_TIME.h"
#include "log.h"

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(logTXT *log, QWidget *parent = nullptr);
    ~LoginWindow();

    logTXT *logtxt;

private slots:
    void onLoginButtonClicked();

private:
    QLabel *usernameLabel;
    QLabel *passwordLabel;
    QLineEdit *usernameLineEdit;
    QLineEdit *passwordLineEdit;
    QPushButton *loginButton;
    QHBoxLayout *usernameLayout;
    QHBoxLayout *passwordLayout;
    QVBoxLayout *mainLayout;

    AdminWindow *adminWindow;
    StudentWindow *studentWindow;

    QVBoxLayout *buttonLayout; // 新增一个按钮布局

    void authenticate(const QString &username, const QString &password);
};

#endif // LOGINWINDOW_H
