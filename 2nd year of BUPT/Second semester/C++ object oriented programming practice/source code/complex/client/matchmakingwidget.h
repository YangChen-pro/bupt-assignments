#ifndef MATCHMAKINGWIDGET_H
#define MATCHMAKINGWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include "client.h"

class MatchmakingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MatchmakingWidget(Client *client, QWidget *parent = nullptr);
    ~MatchmakingWidget();

    bool playing = false;

public slots:
    void getName(const QJsonObject &userInfo);
    void onBackButtonClicked();
    void updateOnlinePlayers(const QStringList &onlinePlayers);

    void handleChallengeReceived(const QString &challenger);
    void handleChallengeResponseReceived(const QString &challengedPlayer, bool accepted);

    void challengePlayerRequest(const QString &playerName);
    void enterBattleslot(const QString &opponentName);

signals:
    void challengePlayerRequested(const QString &playerName);
    void backButtonClicked();

    void enterBattle(const QString &challenger);

private:
    Client *m_client;
    QString m_name;
    QPushButton m_backButton;
    QTreeWidget *m_onlinePlayersTree; // 更改为 QTreeWidget
    QVBoxLayout *m_listLayout;

};

#endif // MATCHMAKINGWIDGET_H
