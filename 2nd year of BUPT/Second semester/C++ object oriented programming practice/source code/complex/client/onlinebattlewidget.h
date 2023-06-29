// OnlineBattleWidget.h
#pragma once

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include "Client.h"

class OnlineBattleWidget : public QWidget
{
    Q_OBJECT

public:
    OnlineBattleWidget(Client *client, QWidget *parent = nullptr);
    ~OnlineBattleWidget();

signals:
    void backButtonClicked();

private slots:
    void handleBattleStart(const QJsonObject &battleInfo);
    void handleBattleWord(const QString &word);
    void handleBattleResult(const QJsonObject &resultInfo);
    void handleBattleEnd(const QJsonObject &endInfo);
    void onSubmit();

private:
    void onBackButtonClicked();

    Client *m_client;
    QLabel m_player1Info;
    QLabel m_player2Info;
    QLabel m_wordLabel;
    QLineEdit m_wordInput;
    QPushButton m_submitButtion;
    QPushButton m_backButton;
    QVBoxLayout m_layout;
    QString opponentName;
    int loseCount = 0;
};
