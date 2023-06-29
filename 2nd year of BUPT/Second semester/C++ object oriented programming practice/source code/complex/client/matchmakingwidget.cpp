#include "MatchmakingWidget.h"

MatchmakingWidget::MatchmakingWidget(Client *client, QWidget *parent) :
    QWidget(parent),
    m_client(client),
    m_backButton("返回"),
    m_onlinePlayersTree(new QTreeWidget(this)), // 更改为 QTreeWidget
    m_listLayout(new QVBoxLayout(this))
{
    m_listLayout->addWidget(m_onlinePlayersTree);
    m_listLayout->addWidget(&m_backButton);
    setLayout(m_listLayout);

    m_onlinePlayersTree->setHeaderLabels({"玩家", "操作"}); // 设置表头
    connect(m_client, &Client::userInfoReceived, this, &MatchmakingWidget::getName);

    connect(&m_backButton, &QPushButton::clicked, this, &MatchmakingWidget::onBackButtonClicked);
    connect(m_client, &Client::onlinePlayersReceived, this, &MatchmakingWidget::updateOnlinePlayers);

    connect(m_client, &Client::challengeReceived, this, &MatchmakingWidget::handleChallengeReceived);
    connect(m_client, &Client::challengeResponseReceived, this, &MatchmakingWidget::handleChallengeResponseReceived);

    connect(this, &MatchmakingWidget::challengePlayerRequested, this, &MatchmakingWidget::challengePlayerRequest);
    connect(this, &MatchmakingWidget::enterBattle, this, &MatchmakingWidget::enterBattleslot);
}

void MatchmakingWidget::getName(const QJsonObject &userInfo)
{
    // 使用 userInfo 更新当前客户端的名字
    m_name = userInfo["username"].toString();
}

MatchmakingWidget::~MatchmakingWidget()
{
}

void MatchmakingWidget::onBackButtonClicked(){
    emit backButtonClicked();
}

void MatchmakingWidget::challengePlayerRequest(const QString &playerName){
    if (playerName == m_name)
    {
        QMessageBox::warning(this, "警告", "不能挑战自己");
        return;
    }
    m_client->sendChallengeRequest(playerName);
}

void MatchmakingWidget::updateOnlinePlayers(const QStringList &onlinePlayers)
{
    m_onlinePlayersTree->clear();

    for (const QString &playerName : onlinePlayers)
    {
        QTreeWidgetItem *playerItem = new QTreeWidgetItem(m_onlinePlayersTree);
        playerItem->setText(0, playerName); // 设置玩家名字

        QPushButton *challengeButton = new QPushButton("发起挑战", this);
        m_onlinePlayersTree->setItemWidget(playerItem, 1, challengeButton); // 设置挑战按钮
        connect(challengeButton, &QPushButton::clicked, this, [this, playerName](){
            emit challengePlayerRequested(playerName);
        });
    }
}

void MatchmakingWidget::handleChallengeReceived(const QString &challenger)
{
    bool accepted;
    if (playing == true)
    {
        accepted = false;
        m_client->sendChallengeResponse(challenger, accepted);
        return;
    }
    QMessageBox challengeBox(this);
    challengeBox.setWindowTitle("挑战请求");
    challengeBox.setText(QString("%1 向您发起挑战！").arg(challenger));
    challengeBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    challengeBox.setDefaultButton(QMessageBox::No);
    challengeBox.setButtonText(QMessageBox::Yes, "接受");
    challengeBox.setButtonText(QMessageBox::No, "拒绝");

    // 创建一个QTimer
    QTimer timer;
    timer.setSingleShot(true); // 设置为单次触发

    // 连接QTimer的timeout信号到QMessageBox的reject槽，以便在超时时自动关闭QMessageBox
    QObject::connect(&timer, &QTimer::timeout, &challengeBox, &QMessageBox::reject);

    // 启动计时器，设置超时时间为10000毫秒（10秒）
    timer.start(10000);

    int result = challengeBox.exec();
    accepted = (result == QMessageBox::Yes);

    m_client->sendChallengeResponse(challenger, accepted);
    if (accepted)
        emit enterBattle(challenger);
}

void MatchmakingWidget::handleChallengeResponseReceived(const QString &challengedPlayer, bool accepted)
{
    if (accepted)
    {
        QMessageBox::information(this, "挑战已接受", QString("%1 已接受您的挑战。").arg(challengedPlayer));
        emit enterBattle(challengedPlayer); // 发送信号进入在线对战界面
    }
    else
    {
        QMessageBox::information(this, "挑战已拒绝", QString("%1 拒绝了您的挑战。").arg(challengedPlayer));
    }
}

void MatchmakingWidget::enterBattleslot(const QString &opponentName){
    playing = true;
    QTimer::singleShot(1000, [this, opponentName]() {
        m_client->sendBattleStart(opponentName);
    });
}
