#include "mainwidget.h"

MainWidget::MainWidget(Client *client, QWidget *parent) :
    QWidget(parent),
    m_client(client),
    m_welcomeLabel("欢迎"),
    m_usernameLabel("用户名："),
    m_userLevelLabel("等级："),
    m_userExpLabel("经验："),
    m_userMaxLevel("最大闯关数："),
    m_onlineGameButton("在线游戏"),
    m_startGameButton("开始游戏"),
    m_leaderboardButton("排行榜"),
    m_viewAllUsersButton("查询所有用户"),
    m_logoutButton("退出登录")
{
    m_layout.addWidget(&m_welcomeLabel);
    m_layout.addWidget(&m_usernameLabel);
    m_layout.addWidget(&m_userLevelLabel);
    m_layout.addWidget(&m_userExpLabel);
    m_layout.addWidget(&m_userMaxLevel);
    m_layout.addWidget(&m_onlineGameButton);
    m_layout.addWidget(&m_startGameButton);
    m_layout.addWidget(&m_leaderboardButton);
    m_layout.addWidget(&m_logoutButton);
    setLayout(&m_layout);

    connect(&m_onlineGameButton, &QPushButton::clicked, this, &MainWidget::onOnlineGameClicked);
    connect(&m_logoutButton, &QPushButton::clicked, this, &MainWidget::onLogoutButtonClicked);
    connect(&m_startGameButton, &QPushButton::clicked, this, &MainWidget::onStartGameButtonClicked);
    connect(&m_leaderboardButton, &QPushButton::clicked, this, &MainWidget::onLeaderboardButtonClicked);
    //connect(&m_viewAllUsersButton, &QPushButton::clicked, this, &MainWidget::onViewAllUsersButtonClicked);

    connect(m_client, &Client::userInfoReceived, this, &MainWidget::updateUserInfo);
}

void MainWidget::updateUserInfo(const QJsonObject &userInfo)
{
    // 使用 userInfo 更新界面
    m_usernameLabel.setText("用户名：" + userInfo["username"].toString());
    m_userLevelLabel.setText("等级：" + QString::number(userInfo["level"].toInt()));
    m_userExpLabel.setText("经验：" + QString::number(userInfo["exp"].toInt()));
    m_userMaxLevel.setText("最大闯关数：" + QString::number(userInfo["maxStage"].toInt()));
}

void MainWidget::onLogoutButtonClicked()
{
    // 退出登录，隐藏主界面，显示登录界面
    emit logout();
}

void MainWidget::onStartGameButtonClicked()
{
    // 显示角色选择界面
    emit startGame();
}

void MainWidget::onLeaderboardButtonClicked()
{
    // 显示排行榜
    emit board();
}

void MainWidget::onOnlineGameClicked()
{
    // 在线游戏界面
    emit onlineGame();
}
