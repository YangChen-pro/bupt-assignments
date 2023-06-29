#include "server.h"

Server::Server(QObject *parent)
    : QObject(parent), m_server(new QTcpServer(this))
{
    threadPool = new QThreadPool(this);
    threadPool->setMaxThreadCount(10);     // 设置线程池最大线程数为10
    connect(m_server, &QTcpServer::newConnection, this, &Server::handleNewConnection);
}

Server::~Server()
{
    stop();
}

bool Server::start(int port)
{
    if (m_server->isListening())
    {
        return false;
    }

    QHostAddress localHostAddress("127.0.0.1");
    bool isListening = m_server->listen(localHostAddress, port);

    if (isListening)
    {
        m_address = m_server->serverAddress();
        m_port = m_server->serverPort();
        qDebug() << "服务器已启动，监听地址：" << m_address.toString() << "端口：" << m_port;
    }
    else
    {
        qWarning() << "无法启动服务器：" << m_server->errorString();
    }

    return isListening;
}

void Server::stop()
{
    for (auto socket : m_games.keys())
    {
        socket->disconnectFromHost();
    }

    m_server->close();
}

QVector<QString> Server::getOnlinePlayers() const {
    QVector<QString> onlinePlayers;
    for (const auto& game : m_games) {
        QString username = game->getCurrentUser()->getUsername();
        onlinePlayers.push_back(username);
    }
    return onlinePlayers;
}

void Server::handleIncomingData()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (!clientSocket)
    {
        return;
    }

    QByteArray data = clientSocket->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    QJsonObject jsonObj = jsonDoc.object();

    int opcode = jsonObj["opcode"].toInt();
    QJsonObject responseData;

    if (opcode == 1) // 登录请求
    {
        QString username = jsonObj["data"].toObject().value("username").toString();
        QString password = jsonObj["data"].toObject().value("password").toString();

        bool loginResult = m_games[clientSocket]->login(username, password);

        responseData["opcode"] = 1;
        responseData["status"] = loginResult ? 0 : 1;
        responseData["message"] = loginResult ? "登录成功" : "登录失败";
    }
    else if (opcode == 2) // 注册请求
    {
        QString username = jsonObj["data"].toObject().value("username").toString();
        QString password = jsonObj["data"].toObject().value("password").toString();

        bool registerResult = m_games[clientSocket]->registerUser(username, password);

        responseData["opcode"] = 2;
        responseData["status"] = registerResult ? 0 : 1;
        responseData["message"] = registerResult ? "注册成功" : "注册失败";
    }
    else if (opcode == 3) // 获取用户信息请求
    {
        // 获取当前用户信息，并将其存储在 QJsonObject userInfo 中
        QJsonObject userInfo;
        // 填充 userInfo
        userInfo["username"] = m_games[clientSocket]->getCurrentUser()->getUsername();
        userInfo["level"] = m_games[clientSocket]->getCurrentUser()->getLevel();
        userInfo["exp"] = m_games[clientSocket]->getCurrentUser()->getExp();
        userInfo["maxStage"] = m_games[clientSocket]->getCurrentUser()->getMaxStage();
        userInfo["lastStage"] = m_games[clientSocket]->getCurrentUser()->getPassedStage();
        userInfo["createdWords"] = m_games[clientSocket]->getCurrentUser()->getCreatedWords();
        userInfo["stage"] = m_games[clientSocket]->getCurrentStage();
        responseData["opcode"] = 3;
        responseData["status"] = 0;
        responseData["userInfo"] = userInfo;
    }
    else if (opcode == 4) // 添加单词请求
    {
        QString word = jsonObj["data"].toObject().value("word").toString();

        bool addWordResult = m_games[clientSocket]->addWord(word);

        responseData["opcode"] = 4;
        responseData["status"] = addWordResult ? 0 : 1;
        responseData["message"] = addWordResult ? "单词添加成功" : "单词已存在，请尝试添加其他单词";
    }
    else if (opcode == 5) // 检查单词正确性请求
    {
        QString inputWord = jsonObj["data"].toObject().value("inputWord").toString();
        bool checkResult = m_games[clientSocket]->submitWord(inputWord);

        responseData["opcode"] = 5;
        responseData["status"] = checkResult ? 0 : 1;
        responseData["message"] = checkResult ? "单词正确" : "单词错误";
    }
    else if (opcode == 6) // 获取新单词请求
    {
        int wordIndex = jsonObj["wordIndex"].toInt();
        QString newWord = m_games[clientSocket]->getWordAt(wordIndex);
        responseData["opcode"] = 6;
        responseData["status"] = 0;
        responseData["newWord"] = newWord;
    }
    else if (opcode == 7) // 进入下一关请求
    {
        m_games[clientSocket]->moveToNextStage();

        responseData["opcode"] = 7;
        responseData["status"] = 0;
    }
    else if (opcode == 8) // 获取倒计时请求
    {
        int timeLeft = (m_games[clientSocket]->getWordDisplayTime());

        responseData["opcode"] = 8;
        responseData["status"] = 0;
        responseData["timeLeft"] = timeLeft;
    }
    else if (opcode == 9) // 从第stage关卡开始
    {
        int stage = jsonObj["stage"].toInt();
        m_games[clientSocket]->startStage(stage);
        responseData["opcode"] = 9;
        responseData["status"] = 0;
    }
    else if (opcode == 10) // 倒计时开始
    {
        m_games[clientSocket]->startCountdown();
        responseData["opcode"] = 10;
        responseData["status"] = 0;
    }
    else if (opcode == 11) // 获取排行榜请求
    {
        int sortOrder = jsonObj["data"].toInt();
        m_games[clientSocket]->fetchLeaderboardData();
        m_games[clientSocket]->sortLeaderboardData(sortOrder);
        QJsonArray sortedLeaderboardData = m_games[clientSocket]->getLeaderboardData();
        responseData["opcode"] = 11;
        responseData["status"] = 0; // 正常情况，返回排序后的排行榜数据
        responseData["data"] = sortedLeaderboardData;
    }
    else if (opcode == 12) // 获取在线玩家列表请求
    {
        QVector<QString> onlinePlayers = getOnlinePlayers();
        QJsonArray onlinePlayersJsonArray = QJsonArray::fromStringList(onlinePlayers);

        responseData["opcode"] = 12;
        responseData["status"] = 0;
        responseData["onlinePlayers"] = onlinePlayersJsonArray;
    }
    else if (opcode == 13) // 收到挑战请求
    {
        QString challengedPlayer = jsonObj["challengedPlayer"].toString();
        handleChallengeRequest(clientSocket, challengedPlayer);
    }
    else if (opcode == 14) // 收到挑战响应
    {
        QString challengingPlayer = jsonObj["challengingPlayer"].toString();
        bool accepted = jsonObj["accepted"].toBool();
        handleChallengeResponse(clientSocket, challengingPlayer, accepted);
    }
    else if (opcode == 20) // 收到 battleStart 请求
    {
        QString opponentPlayer = jsonObj["opponent"].toString();
        handleBattleStart(clientSocket, opponentPlayer, jsonObj);
        return;
    }
    else if (opcode == 21) // 收到 getBattleWord 请求
    {
        handleGetBattleWord(clientSocket, jsonObj);
        return;
    }
    else if (opcode == 22) // 收到 battleInput 请求
    {
        handleBattleInput(clientSocket, jsonObj);
        return;
    }
    else if (opcode == 23) // 收到 battleEnd 请求
    {
        handleBattleEnd(clientSocket, jsonObj);
        return;
    }
    // 发送响应给客户端
    if (opcode != 13 && opcode != 14) // 13和14是向另一个客户端发数据
    {
        QByteArray response = QJsonDocument(responseData).toJson();
        clientSocket->write(response);
    }
}


void Server::handleNewConnection()
{
    QTcpSocket *clientConnection = m_server->nextPendingConnection();
    connect(clientConnection, &QTcpSocket::disconnected, this, &Server::handleClientDisconnected);
    connect(clientConnection, &QTcpSocket::readyRead, this, &Server::handleIncomingData);

    m_games.insert(clientConnection, new Game());

}

void Server::handleClientDisconnected()
{
    QTcpSocket *clientConnection = qobject_cast<QTcpSocket *>(sender());
    if (clientConnection)
    {
        delete m_games.take(clientConnection);
        clientConnection->deleteLater();
    }
}

void Server::handleReadyRead()
{
    QTcpSocket *clientConnection = qobject_cast<QTcpSocket *>(sender());
    if (clientConnection)
    {
        QByteArray requestData = clientConnection->readAll();
        parseRequest(clientConnection, requestData);
    }
}

void Server::parseRequest(QTcpSocket *socket, const QByteArray &requestData)
{
    // 解析请求数据并调用相应的处理函数
    // ...
}

QTcpSocket *Server::findSocketByUsername(const QString &username)
{
    for (auto it = m_games.begin(); it != m_games.end(); ++it)
    {
        if (it.value()->getCurrentUser()->getUsername() == username)
        {
            return it.key();
        }
    }
    return nullptr;
}

void Server::handleChallengeRequest(QTcpSocket *clientSocket, const QString &challengedPlayer)
{
    QTcpSocket *challengedSocket = findSocketByUsername(challengedPlayer);
    if (challengedSocket)
    {
        // 发送挑战请求给被挑战者
        QJsonObject requestData;
        requestData["opcode"] = 13;
        requestData["challenger"] = m_games[clientSocket]->getCurrentUser()->getUsername();

        QByteArray data = QJsonDocument(requestData).toJson();
        challengedSocket->write(data);
    }
}

void Server::handleChallengeResponse(QTcpSocket *clientSocket, const QString &challengingPlayer, bool accepted)
{
    QTcpSocket *challengerSocket = findSocketByUsername(challengingPlayer);
    if (challengerSocket)
    {
        // 发送挑战响应给挑战者
        QJsonObject responseData;
        responseData["opcode"] = 14;
        responseData["accepted"] = accepted;
        responseData["challengedPlayer"] = m_games[clientSocket]->getCurrentUser()->getUsername();

        QByteArray data = QJsonDocument(responseData).toJson();
        challengerSocket->write(data);
    }
}


void Server::handleBattleStart(QTcpSocket *clientSocket, const QString &opponentPlayer, const QJsonObject &request)
{
    QTcpSocket *opponentSocket = findSocketByUsername(opponentPlayer);
    QJsonObject responseData;
    responseData["opcode"] = 20;
    responseData["status"] = 1;

    responseData["player1"] = m_games[clientSocket]->getCurrentUser()->getUsername(); // 从游戏中获取玩家名
    responseData["player1Level"] = m_games[clientSocket]->getCurrentUser()->getLevel();
    responseData["player1Exp"] = m_games[clientSocket]->getCurrentUser()->getExp();
    responseData["player2"] = m_games[opponentSocket]->getCurrentUser()->getUsername();
    responseData["player2Level"] = m_games[opponentSocket]->getCurrentUser()->getLevel();
    responseData["player2Exp"] = m_games[opponentSocket]->getCurrentUser()->getExp();


    if (m_battleWords[clientSocket].isEmpty())
    {
        auto words = m_games[clientSocket]->chooseWordsFromPool(10, 3);
        QVector<QString> qWords;
        for (const auto &word : words)
        {
            qWords.append(word);
        }
        m_battleWords[clientSocket] = qWords;
        m_battleWords[opponentSocket] = qWords;
    }
    QByteArray response = QJsonDocument(responseData).toJson();
    clientSocket->write(response);
}

void Server::handleGetBattleWord(QTcpSocket *clientSocket, const QJsonObject &request)
{
    QJsonObject responseData;
    responseData["opcode"] = 21;

    if (!m_battleWords.contains(clientSocket))
    {
        responseData["status"] = 0;
        responseData["message"] = "未开始对战";
    }
    else
    {
        responseData["status"] = 1;
        responseData["word"] = m_battleWords[clientSocket].front();
    }

    QByteArray response = QJsonDocument(responseData).toJson();
    clientSocket->write(response);
}

void Server::handleBattleInput(QTcpSocket *clientSocket, const QJsonObject &request)
{
    QJsonObject responseData;
    responseData["opcode"] = 22;
    QString inputWord = request["word"].toString();
    QString opponentName = request["opponent"].toString();
    QTcpSocket *opponentSocket = findSocketByUsername(opponentName);

    responseData["round"] = 4 - m_battleWords[clientSocket].size();

    if (m_battleWords[clientSocket].isEmpty() || inputWord != m_battleWords[clientSocket].front())
    {
        responseData["status"] = 0;
        responseData["winner"] = m_games[opponentSocket]->getCurrentUser()->getUsername();
    }
    else if (inputWord == m_battleWords[clientSocket].front())
    {
        responseData["status"] = 1;
        responseData["winner"] = m_games[clientSocket]->getCurrentUser()->getUsername();

        m_battleWords[clientSocket].pop_front();

        m_battleWords[opponentSocket].pop_front();
    }

    QByteArray response = QJsonDocument(responseData).toJson();
    clientSocket->write(response);
    // 一方赢了，另一方直接失败
    if (responseData["status"].toInt() == 1)
    {
        responseData["status"] = 0;
        QByteArray opponentResponse = QJsonDocument(responseData).toJson();
        opponentSocket->write(opponentResponse);
    }
}

void Server::handleBattleEnd(QTcpSocket *clientSocket, const QJsonObject &request)
{
    bool won = request["won"].toBool();
    QJsonObject responseData;
    if (won)
    {
        m_games[clientSocket]->getCurrentUser()->gainExp(500);
        responseData["winner"] = m_games[clientSocket]->getCurrentUser()->getUsername();
    }
    else
    {
        m_games[clientSocket]->getCurrentUser()->gainExp(-300);
        responseData["loser"] = m_games[clientSocket]->getCurrentUser()->getUsername();
    }

    m_games[clientSocket]->updateUserInDatabase();


    responseData["opcode"] = 23;
    responseData["status"] = 1;


    QByteArray response = QJsonDocument(responseData).toJson();
    clientSocket->write(response);
}
