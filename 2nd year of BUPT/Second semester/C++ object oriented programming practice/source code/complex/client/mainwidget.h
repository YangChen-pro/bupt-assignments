#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "client.h"

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(Client *client, QWidget *parent = nullptr);

signals:
    void logout();
    void startGame();
    void board();
    void onlineGame();

public slots:
    void updateUserInfo(const QJsonObject &userInfo);

private:
    Client *m_client;
    QLabel m_welcomeLabel;
    QLabel m_usernameLabel;
    QLabel m_userLevelLabel;
    QLabel m_userExpLabel;
    QLabel m_userMaxLevel;
    QPushButton m_onlineGameButton;
    QPushButton m_startGameButton;
    QPushButton m_leaderboardButton;
    QPushButton m_viewAllUsersButton;
    QPushButton m_logoutButton;
    QVBoxLayout m_layout;

private slots:
    void onLogoutButtonClicked();
    void onStartGameButtonClicked();
    void onLeaderboardButtonClicked();
    void onOnlineGameClicked();
};

#endif // MAINWIDGET_H
