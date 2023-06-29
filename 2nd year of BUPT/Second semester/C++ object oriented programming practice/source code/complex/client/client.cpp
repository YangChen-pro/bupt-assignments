#include "client.h"

Client::Client(QObject *parent)
    : QObject(parent), m_socket(new QTcpSocket(this))
{
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::handleReadyRead);
}

Client::~Client()
{
    disconnectFromServer();
}

void Client::connectToServer(const QString &hostname, int port)
{
    // 连接成功时的输出
    connect(m_socket, &QTcpSocket::connected, []() {
        qDebug() << "客户端已成功连接到服务器。";
    });

    // 连接失败时的输出
    connect(m_socket, &QTcpSocket::errorOccurred, [this]() {
        qDebug() << "客户端连接到服务器时发生错误：" << m_socket->errorString();
    });

    m_socket->connectToHost(hostname, port);
}

void Client::disconnectFromServer()
{
    m_socket->disconnectFromHost();
}

void Client::sendLoginRequest(const QString &username, const QString &password)
{
    QJsonObject requestData;
    requestData["opcode"] = 1; // 登录请求
    requestData["data"] = QJsonObject{
        {"username", username},
        {"password", password}
    };

    QByteArray data = QJsonDocument(requestData).toJson();
    m_socket->write(data);
    qDebug() << "登录请求已发送。";
}

void Client::sendRegisterRequest(const QString &username, const QString &password)
{
    QJsonObject requestData;
    requestData["opcode"] = 2; // 注册请求
    requestData["data"] = QJsonObject{
        {"username", username},
        {"password", password}
    };

    QByteArray data = QJsonDocument(requestData).toJson();
    m_socket->write(data);
    qDebug() << "注册请求已发送。";
}

void Client::sendGetUserInfoRequest()
{
    QJsonObject requestData;
    requestData["opcode"] = 3; // 获取用户信息请求

    QByteArray data = QJsonDocument(requestData).toJson();
    m_socket->write(data);
    qDebug() << "获取用户信息请求已发送。";
}

void Client::sendAddWordRequest(const QString &word)
{
    QJsonObject requestData;
    requestData["opcode"] = 4; // 添加单词请求
    requestData["data"] = QJsonObject{
        {"word", word}
    };

    QByteArray data = QJsonDocument(requestData).toJson();
    m_socket->write(data);
    qDebug() << "添加单词请求已发送。";
}

void Client::sendCheckWordRequest(const QString &inputWord)
{
    QJsonObject requestData;
    requestData["opcode"] = 5; // 检查单词正确性请求
    requestData["data"] = QJsonObject{
        {"inputWord", inputWord}
    };

    QByteArray data = QJsonDocument(requestData).toJson();
    m_socket->write(data);
    qDebug() << "检查单词正确性请求已发送。";
}

void Client::sendGetNewWordRequest(int wordIndex)
{
    QJsonObject requestData;
    requestData["opcode"] = 6; // 获取新单词请求
    requestData["wordIndex"] = wordIndex;

    QByteArray data = QJsonDocument(requestData).toJson();
    m_socket->write(data);
    qDebug() << "获取新单词请求已发送。";
}

void Client::sendMoveToNextStageRequest()
{
    QJsonObject requestData;
    requestData["opcode"] = 7; // 进入下一关请求

    QByteArray data = QJsonDocument(requestData).toJson();
    m_socket->write(data);
    qDebug() << "进入下一关请求已发送。";
}

void Client::sendGetRemainingTimeRequest()
{
    QJsonObject requestData;
    requestData["opcode"] = 8; // 获取倒计时请求

    QByteArray data = QJsonDocument(requestData).toJson();
    m_socket->write(data);
    qDebug() << "获取倒计时请求已发送。";
}

void Client::sendStartGame(int stage)
{
    QJsonObject requestData;
    requestData["opcode"] = 9; // 请求从stage开始
    requestData["stage"] = stage;
    QByteArray data = QJsonDocument(requestData).toJson();
    m_socket->write(data);
    qDebug() << "从" + QString::number(stage) + "关开始的请求已发送";
}

void Client::sendStartCountdown()
{
    QJsonObject requestData;
    requestData["opcode"] = 10;
    QByteArray data = QJsonDocument(requestData).toJson();
    m_socket->write(data);
    qDebug() << "倒计时开始的请求已发送";
}

void Client::sendSortLeaderboardRequest(int sortOrder)
{
    QJsonObject requestData;
    requestData["opcode"] = 11; // 排行榜排序请求
    requestData["data"] = sortOrder;

    QByteArray data = QJsonDocument(requestData).toJson();
    m_socket->write(data);
    qDebug() << "排行榜排序请求已发送。";
}

void Client::sendGetOnlinePlayersRequest()
{
    QJsonObject requestData;
    requestData["opcode"] = 12; // 获取在线玩家列表请求

    QByteArray data = QJsonDocument(requestData).toJson();
    m_socket->write(data);
    qDebug() << "在线玩家列表请求已发送。";
}

void Client::sendChallengeRequest(const QString &challengedPlayer)
{
    QJsonObject requestData;
    requestData["opcode"] = 13; // 挑战请求
    requestData["challengedPlayer"] = challengedPlayer;

    QByteArray data = QJsonDocument(requestData).toJson();
    m_socket->write(data);
    qDebug() << "向" + challengedPlayer + "发起挑战";
}

void Client::sendChallengeResponse(const QString &challenger, bool accepted)
{
    QJsonObject requestData;
    requestData["opcode"] = 14; // 挑战响应
    requestData["challengingPlayer"] = challenger;
    requestData["accepted"] = accepted;

    QByteArray data = QJsonDocument(requestData).toJson();
    qDebug() << "向" + challenger + "发送回应";
    m_socket->write(data);
}

void Client::sendBattleStart(const QString &opponent)
{
    QJsonObject requestData;
    requestData["opcode"] = 20;
    requestData["opponent"] = opponent;
    QByteArray data = QJsonDocument(requestData).toJson();
    m_socket->write(data);
    qDebug() << "发送 battleStart 请求，获取双方信息";
}

void Client::sendGetBattleWord()
{
    QJsonObject requestData;
    requestData["opcode"] = 21; // getBattleWord
    QByteArray data = QJsonDocument(requestData).toJson();
    m_socket->write(data);
    qDebug() << "发送 getBattleWord 请求";
}

void Client::sendBattleInput(const QString &word, const QString &opponentName)
{
    QJsonObject requestData;
    requestData["opcode"] = 22; // battleInput
    requestData["word"] = word;
    requestData["opponent"] = opponentName;
    QByteArray data = QJsonDocument(requestData).toJson();
    m_socket->write(data);
    qDebug() << "发送 battleInput：" << word;
}

void Client::sendBattleEnd(bool won)
{
    QJsonObject requestData;
    requestData["opcode"] = 23; // battleEnd
    requestData["won"] = won;
    QByteArray data = QJsonDocument(requestData).toJson();
    m_socket->write(data);
    qDebug() << "发送 battleEnd 请求";
}

void Client::handleReadyRead()
{
    qDebug() << "收到服务器响应";
    QByteArray data = m_socket->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    QJsonObject jsonObj = jsonDoc.object();
    handleServerResponse(jsonObj);
}

void Client::handleServerResponse(const QJsonObject &response)
{
    int opcode = response["opcode"].toInt();
    int status = response["status"].toInt();
    QString message = response["message"].toString();

    switch(opcode)
    {
    case 1: // 登录响应
        if (status == 0)
        {
            qDebug() << "登录成功：" << message;
            emit loginSucceeded();
        }
        else
        {
            qDebug() << "登录失败：" << message;
            emit loginFailed(message);
        }
        break;

    case 2: // 注册响应
        if (status == 0)
        {
            qDebug() << "注册成功：" << message;
            emit registerSucceeded();
        }
        else
        {
            qDebug() << "注册失败：" << message;
            emit registerFailed(message);
        }
        break;

    case 3: // 获取用户信息响应
        if (status == 0)
        {
            QJsonObject userInfo = response["userInfo"].toObject();
            qDebug() << "用户信息已接收。";
            emit userInfoReceived(userInfo);
        }
        else
        {
            qDebug() << "获取用户信息失败：" << message;
        }
        break;

    case 4: // 添加单词响应
        if (status == 0)
        {
            qDebug() << "添加单词成功：" << message;
            emit wordAddedSuccessfully();
        }
        else
        {
            qDebug() << "添加单词失败：" << message;
            emit wordAdditionFailed(message);
        }
        break;
    case 5: // 单词正确性相应
        if (status == 0)
        {
            qDebug() << "单词正确" << message;
            emit wordAccept();
        }
        else
        {
            qDebug() << "单词错误" << message;
            emit wordWrong(message);
        }
        break;
    case 6: // 获取新单词请求
    {
        QString newWord = response["newWord"].toString();
        qDebug() << newWord;
        emit getNewWord(newWord);
        break;
    }
    case 7: // 进入下一关响应
    {
        if (status == 0)
        {
            qDebug() << "进入下一关";
            emit stageChanged();
        }
        break;
    }
    case 8: // 获取倒计时响应
    {
        int timeLeft = response["timeLeft"].toInt();
        qDebug() << "倒计时剩余时间: " << timeLeft << "s";
        emit remainingTimeChanged(timeLeft);
        break;
    }
    case 9: // 从stage开始相应
    {
        if (status == 0)
            qDebug() << "服务器已相应，从指定关卡开始";
        emit startStage();
        break;

    }
    case 10: // 倒计时开始相应
    {
        if (status == 0)
            qDebug() << "服务器已相应，倒计时开始";
        emit startCountdown();
        break;

    }
    case 11:
    {
        if (status == 0)
            qDebug() << "排行榜数据已获取";
        QJsonArray sortedLeaderboardData = response["data"].toArray();
        emit sortedData(sortedLeaderboardData);
        break;
    }
    case 12:
    {
        if (status == 0)
            qDebug() << "在线玩家列表已获取";
        QJsonArray onlinePlayersJsonArray = response["onlinePlayers"].toArray();
        QStringList onlinePlayers;
        for (const QJsonValue &value : onlinePlayersJsonArray) {
            onlinePlayers.append(value.toString());
        }
        emit onlinePlayersReceived(onlinePlayers);
        break;
    }
    case 13: // 挑战请求
    {
        QString challenger = response["challenger"].toString();
        qDebug() << "收到来自" + challenger + "的挑战请求";
        emit challengeReceived(challenger);
        break;
    }
    case 14: // 挑战响应
    {
        QString challengedPlayer = response["challengedPlayer"].toString();
        qDebug() << "收到来自" + challengedPlayer + "的回应";
        bool accepted = response["accepted"].toBool();
        emit challengeResponseReceived(challengedPlayer, accepted);
        break;
    }
    case 20: // 开始2人对战
    {
        if (status == 1) // 成功
        {
            qDebug() << "收到双方信息";
            emit battleStartReceived(response);
        }
        else
        {
            // 处理失败情况
        }
        break;
    }
    case 21: // 获取对战单词
    {
        if (status == 1) // 成功
        {
            QString word = response["word"].toString();
            qDebug() << "已获取到新单词" << word;
            emit battleWordReceived(word);
        }
        else
        {
            qDebug() << "未获取到对战单词";
            // 处理失败情况
        }
        break;
    }
    case 22: // 对战的单词提交后，收到反馈
    {
        if (status == 1) // 成功
        {
            qDebug() << "先一步提交成功";
            emit battleResultReceived(response);
        }
        else // 处理失败情况
        {
            qDebug() << "提交错误或超时";
            emit battleResultReceived(response);
        }
        break;
    }
    case 23: // 对战结束
    {
        if (status == 1) // 成功
        {
            emit battleEndReceived(response);
        }
        else
        {
            // 处理失败情况
        }
        break;
    }
    default:
        qDebug() << "未知操作码：" << opcode;
    }
}
