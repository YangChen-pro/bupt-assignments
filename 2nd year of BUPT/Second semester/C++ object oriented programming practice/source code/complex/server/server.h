#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThreadPool>
#include "game.h"

class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    ~Server();

    bool start(int port); // 开始监听客户端连接
    void stop(); // 停止监听客户端连接

    QVector<QString> getOnlinePlayers() const; // 获取在线玩家
    void handleClient(QTcpSocket *clientConnection);


private slots:
    void handleIncomingData();
    void handleNewConnection(); // 处理新连接
    void handleClientDisconnected(); // 处理客户端断开连接
    void handleReadyRead(); // 处理客户端发送的数据

private:
    QThreadPool *threadPool;     // 自己的线程池
    int m_maxThreadCount; // 线程池最大线程数
    QHash<QTcpSocket *, QVector<QString>> m_battleWords;
    QHostAddress m_address;
    quint16 m_port;
    QTcpServer *m_server; // 服务器socket
    QMap<QTcpSocket *, Game *> m_games; // 每个客户端连接对应一个Game实例

    // 解析客户端请求并调用相应的处理函数
    void parseRequest(QTcpSocket *socket, const QByteArray &requestData);

    QTcpSocket* findSocketByUsername(const QString &username);

    void handleChallengeRequest(QTcpSocket *clientSocket, const QString &challengedPlayer);
    void handleChallengeResponse(QTcpSocket *clientSocket, const QString &challengingPlayer, bool accepted);

    void handleBattleStart(QTcpSocket *clientSocket, const QString &opponentPlayer, const QJsonObject &request);
    void handleGetBattleWord(QTcpSocket *clientSocket, const QJsonObject &request);
    void handleBattleInput(QTcpSocket *clientSocket, const QJsonObject &request);
    void handleBattleEnd(QTcpSocket *clientSocket, const QJsonObject &request);
    void sendBattleResult(QTcpSocket *clientSocket, bool won);

};

#endif // SERVER_H
