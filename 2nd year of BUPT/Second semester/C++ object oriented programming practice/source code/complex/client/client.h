#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>
#include <QStringList>

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    ~Client();

    void connectToServer(const QString &hostname, int port);
    void disconnectFromServer();
    void sendLoginRequest(const QString &username, const QString &password);
    void sendRegisterRequest(const QString &username, const QString &password);
    void sendGetUserInfoRequest();
    void sendAddWordRequest(const QString &word);
    void sendCheckWordRequest(const QString &inputWord);
    void sendGetNewWordRequest(int wordIndex);
    void sendMoveToNextStageRequest();
    void sendGetRemainingTimeRequest();
    void sendStartGame(int stage);
    void sendStartCountdown();
    void sendSortLeaderboardRequest(int sortOrder);
    void sendGetOnlinePlayersRequest();

    void sendChallengeRequest(const QString &challengedPlayer);
    void sendChallengeResponse(const QString &challenger, bool accepted);

    void sendBattleStart(const QString &opponent);
    void sendGetBattleWord();
    void sendBattleInput(const QString &word, const QString &opponentName);
    void sendBattleEnd(bool won);

signals:
    void loginSucceeded();
    void loginFailed(const QString& message);
    void registerSucceeded();
    void registerFailed(const QString& message);
    void userInfoReceived(const QJsonObject userInfo);
    void wordAddedSuccessfully();
    void wordAdditionFailed(const QString& message);
    void wordAccept();
    void wordWrong(const QString& message);
    void getNewWord(const QString& newWord);
    void stageChanged();
    void remainingTimeChanged(int timeLeft);
    void stageInfoUpdated(int currentStage, int currentWordCount);
    void startStage();
    void startCountdown();
    void sortedData(QJsonArray sortedLeaderboardData);
    void onlinePlayersReceived(QStringList onlinePlayers);

    void challengeReceived(const QString &challenger);
    void challengeResponseReceived(const QString &challengedPlayer, bool accepted);

    void battleStartReceived(const QJsonObject &battleInfo);
    void battleWordReceived(const QString &word);
    void battleResultReceived(const QJsonObject &resultInfo);
    void battleEndReceived(const QJsonObject &endInfo);

private slots:
    void handleReadyRead();
    void handleServerResponse(const QJsonObject &response);

private:
    QTcpSocket *m_socket;
};

#endif // CLIENT_H
