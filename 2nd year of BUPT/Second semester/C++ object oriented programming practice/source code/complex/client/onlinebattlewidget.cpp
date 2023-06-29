// OnlineBattleWidget.cpp
#include "OnlineBattleWidget.h"
#include <QTimer>

OnlineBattleWidget::OnlineBattleWidget(Client *client, QWidget *parent)
    : QWidget(parent),
      m_client(client),
      m_player1Info("玩家 1"),
      m_player2Info("玩家 2"),
      m_wordLabel("单词"),
      m_wordInput(""),
      m_submitButtion("提交"),
      m_backButton("返回"),
      m_layout(this)
{
    m_layout.addWidget(&m_player1Info);
    m_layout.addWidget(&m_player2Info);
    m_layout.addWidget(&m_wordLabel);
    m_layout.addWidget(&m_wordInput);
    m_layout.addWidget(&m_submitButtion);
    m_layout.addWidget(&m_backButton);
    setLayout(&m_layout);

    connect(&m_backButton, &QPushButton::clicked, this, &OnlineBattleWidget::onBackButtonClicked);
    connect(&m_submitButtion, &QPushButton::clicked, this, &OnlineBattleWidget::onSubmit);

    connect(m_client, &Client::battleStartReceived, this, &OnlineBattleWidget::handleBattleStart);
    connect(m_client, &Client::battleWordReceived, this, &OnlineBattleWidget::handleBattleWord);
    connect(m_client, &Client::battleResultReceived, this, &OnlineBattleWidget::handleBattleResult);
    connect(m_client, &Client::battleEndReceived, this, &OnlineBattleWidget::handleBattleEnd);
}

OnlineBattleWidget::~OnlineBattleWidget()
{
}

void OnlineBattleWidget::onBackButtonClicked()
{
    emit backButtonClicked();
}

void OnlineBattleWidget::onSubmit(){
    QString word = m_wordInput.text();
    m_client->sendBattleInput(word, opponentName);
}

void OnlineBattleWidget::handleBattleStart(const QJsonObject &battleInfo)
{
    // 跟新玩家信息
    m_player1Info.setText(QString("玩家 1: %1 (等级 %2, 经验 %3)")
                              .arg(battleInfo["player1"].toString())
                              .arg(battleInfo["player1Level"].toInt())
                              .arg(battleInfo["player1Exp"].toInt()));
    m_player2Info.setText(QString("玩家 2: %1 (等级 %2, 经验 %3)")
                              .arg(battleInfo["player2"].toString())
                              .arg(battleInfo["player2Level"].toInt())
                              .arg(battleInfo["player2Exp"].toInt()));

    loseCount = 0;
    opponentName = battleInfo["player2"].toString();
    // 开始时不能输入
    m_wordInput.setEnabled(false);
    m_submitButtion.setEnabled(false);
    m_client->sendGetBattleWord();
}

void OnlineBattleWidget::handleBattleWord(const QString &word)
{
    m_wordLabel.setText(word);

    // 单词展示4秒后消失，允许输入
    QTimer::singleShot(4000, [this]() {
        m_wordLabel.clear();
        m_wordInput.setEnabled(true);
        m_submitButtion.setEnabled(true);
    });
}

void OnlineBattleWidget::handleBattleResult(const QJsonObject &resultInfo)
{
    int round = resultInfo["round"].toInt();
    QString winner = resultInfo["winner"].toString();

    // 给出回合结果
    QMessageBox::information(this, "回合结果", QString("回合 %1 胜利者：%2").arg(round).arg(winner));

    if (winner == opponentName)
        loseCount++;
    // 禁用，清空输入框
    m_wordInput.setEnabled(false);
    m_submitButtion.setEnabled(false);
    m_wordInput.clear();

    if (round != 3)
        m_client->sendGetBattleWord();
    else
    {
        if (loseCount >= 2)
            m_client->sendBattleEnd(0);
        else
            m_client->sendBattleEnd(1);
    }
}

void OnlineBattleWidget::handleBattleEnd(const QJsonObject &endInfo)
{
    QString winner = endInfo["winner"].toString();
    QString loser = endInfo["loser"].toString();

    if (winner != "")
        QMessageBox::information(this, "游戏结束", "你获胜了");
    else
        QMessageBox::information(this, "游戏结束", "你失败了");
    // Disable input and clear it
    m_wordInput.setEnabled(false);
    m_submitButtion.setEnabled(false);
    m_wordInput.clear();

    m_client->sendGetUserInfoRequest();
    emit backButtonClicked();
}
