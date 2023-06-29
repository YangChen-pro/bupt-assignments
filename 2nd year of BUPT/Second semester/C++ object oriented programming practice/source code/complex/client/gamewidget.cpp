

#include "gamewidget.h"

GameWidget::GameWidget(Client *client, QWidget *parent)
{
    this->m_client = client;
    setupUi();
    connectSignalsAndSlots();
}

void GameWidget::setupUi()
{
    playernameLabel = new QLabel("玩家名: ");
    playerLevelLabel = new QLabel("玩家等级: ");
    playerExpLabel = new QLabel("玩家经验值: ");

    levelLabel = new QLabel("关卡号: ");
    wordLabel = new QLabel("");
    wordCountLabel = new QLabel("");

    countdownLabel = new QLabel("倒计时: ");
    countdownLabel->setAlignment(Qt::AlignCenter);

    wordInput = new QLineEdit();
    submitButton = new QPushButton("提交");
    backButton = new QPushButton("返回主界面");

    gameLayout = new QVBoxLayout();
    gameLayout->addWidget(playernameLabel);
    gameLayout->addWidget(playerLevelLabel);
    gameLayout->addWidget(playerExpLabel);
    gameLayout->addWidget(levelLabel);
    gameLayout->addWidget(wordLabel);
    gameLayout->addWidget(wordCountLabel);
    gameLayout->addWidget(countdownLabel);
    gameLayout->addWidget(wordInput);
    gameLayout->addWidget(submitButton);
    gameLayout->addWidget(backButton);

    setLayout(gameLayout);

    timer = new QTimer(this);

}

void GameWidget::updatePlayerInfo(const QJsonObject &userInfo)
{
    playernameLabel->setText("玩家名: " + userInfo["username"].toString());
    playerLevelLabel->setText("玩家等级: " + QString::number(userInfo["level"].toInt()));
    playerExpLabel->setText("玩家经验值: " + QString::number(userInfo["exp"].toInt()));
    levelLabel->setText("关卡号: " + QString::number(userInfo["stage"].toInt()));
    currentStage = userInfo["stage"].toInt();
    wordsRequired = (currentStage - 1) / 5 + 1;
    wordCountLabel->setText("当前关卡单词数: " + QString::number(wordsRequired));

    lastStage = userInfo["lastStage"].toInt();
    newLevel = userInfo["level"].toInt();
    newExp = userInfo["exp"].toInt();
    wordInput->setEnabled(false);
    if (startFlag)
    {
        qDebug() << "新关卡开始，需要获取新单词";
        m_client->sendGetNewWordRequest(0);
    }
}

void GameWidget::startGame(int stage)
{
    m_client->sendStartGame(stage);
}

void GameWidget::moveToNext(){
    m_client->sendGetUserInfoRequest();
}

void GameWidget::onStartStage(){
    startFlag = 1;
    m_client->sendGetUserInfoRequest();
    qDebug() << "刷新用户信息";
}

void GameWidget::connectSignalsAndSlots()
{
    connect(submitButton, &QPushButton::clicked, this, &GameWidget::submitSlot);
    connect(backButton, &QPushButton::clicked, this, &GameWidget::backClicked);
    connect(timer, &QTimer::timeout, this, &GameWidget::updateTimer);

    connect(m_client, &Client::userInfoReceived, this, &GameWidget::updatePlayerInfo);
    connect(m_client, &Client::getNewWord, this, &GameWidget::generateWord);
    connect(m_client, &Client::remainingTimeChanged, this, &GameWidget::getRemainingTime);

    connect(m_client, &Client::wordAccept, this, &GameWidget::wordIsAccept);
    connect(m_client, &Client::wordWrong, this, &GameWidget::wordIsWrong);
    connect(m_client, &Client::stageChanged, this, &GameWidget::moveToNext);
    connect(m_client, &Client::startStage, this, &GameWidget::onStartStage);

}

void GameWidget::wordIsAccept(){
    QMessageBox::information(this, "正确", "恭喜你, 答对了!");
    wordCount++;
    if (wordCount >= wordsRequired) {
        wordCount = 0;
        wordIndex = 0;
        m_client->sendMoveToNextStageRequest();
        nextFlag = 1;
    } else {
        wordIndex++;
        nextFlag = 0;
    }
    if (!nextFlag)
    {
        qDebug() << "获取本关卡下一个单词";
        m_client->sendGetNewWordRequest(wordIndex);
    }
    wordInput->clear();
    wordInput->setEnabled(false);
}

void GameWidget::wordIsWrong(const QString& message){
    QMessageBox::warning(this, "错误", "很遗憾, 答案错误。");
        qDebug() << "单词错误，重新获取单词";
    m_client->sendGetNewWordRequest(wordIndex);
    wordInput->clear();
    wordInput->setEnabled(false);
}

void GameWidget::generateWord(const QString& newWord)
{
    wordLabel->setText(newWord);
    qDebug() << newWord;
    wordInput->clear();
    m_client->sendGetRemainingTimeRequest();
}

void GameWidget::getRemainingTime(int timeLeft){
    this->timeLeft = timeLeft;
    timer->start(1000); // 每秒触发一次计时器
    countdownLabel->setText("倒计时: " + QString::number(timeLeft));
}

void GameWidget::submitSlot()
{
    QString inputWord = wordInput->text();
    m_client->sendCheckWordRequest(inputWord);
}

void GameWidget::updateTimer()
{
    qDebug() << "剩余时间" + QString::number(timeLeft) + "秒";
    timeLeft--;
    if (timeLeft <= 0) {
        // 停止计时器
        timer->stop();
        // 清除单词标签
        wordLabel->clear();
        // 倒计时为0
        countdownLabel->setText("倒计时: 0 秒");
        // 允许输入
        wordInput->setEnabled(true);
    } else {
        // 更新倒计时标签
        countdownLabel->setText("倒计时: " + QString::number(timeLeft) + " 秒");
    }
}

void GameWidget::startFromLastStage()
{
    startGame(lastStage + 1);
}

