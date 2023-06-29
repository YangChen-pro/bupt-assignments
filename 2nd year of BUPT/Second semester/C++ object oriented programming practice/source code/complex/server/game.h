#ifndef GAME_H
#define GAME_H

#include <QString>
#include <QSqlDatabase>
#include <vector>
#include <random>
#include <QTimer>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QTime>
#include <QCoreApplication>
#include <algorithm>
#include <random>
#include <chrono>
#include <QVector>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "User.h"

class Game: public QObject
{
    Q_OBJECT
public:
    Game();
    ~Game();
    bool connectToDatabase(); // 连接到数据库
    void closeDatabase(); // 关闭数据库连接
    User* getCurrentUser() const; // 获取用户信息
    int getCurrentStage() const; // 当前关卡号
    int getCurrentWordCount() const; // 当前关卡有多少个单词
    void moveToNextStage(); // 下一关
    QString getWordAt(int wordIndex) const; // 获取单词
    double getRemainingTime() const; // 获取剩余时间
    bool login(const QString& userId, const QString& password);
    bool registerUser(const QString& username, const QString& password);

    void logout(); // 登出

    void selectRole(bool isPlayer);

    void startStage(int stage); // 闯关者从第stage关卡开始
    bool submitWord(const QString& word); // 闯关者提交单词
    bool addWord(const QString& word); // 出题者添加单词

    // 其他你可能需要的功能函数
    double getWordDisplayTime() const; // 获取当前关卡单词展示时间
    int calculateExpGain() const;
    void startCountdown(); // 启动倒计时
    void stopCountdown(); // 停止倒计时

    bool updateUserInDatabase(); // 更新数据库信息

    void fetchLeaderboardData();
    void sortLeaderboardData(int sortBy);

    struct LeaderboardEntry // 排行榜信息
    {
        int id;
        QString username;
        int level;
        int exp;
        int currentLevel;
        int maxLevel;
        int questions;
    };
    QJsonArray getLeaderboardData() const;

    // 从词库中随机选择单词，参数为关卡和单词数目
    std::vector<QString> chooseWordsFromPool(int stage, int wordCount) const;

signals:
    void countdownUpdated(double remainingTime);    // 倒计时更新信号
    void userLoggedIn();


private:
    User* m_currentUser;
    User* createUserFromQuery(const QSqlQuery& query);
    // 随机数生成器
    std::default_random_engine m_randomGenerator;
    QSqlDatabase m_db;
    bool m_isPlayer;
    int m_currentStage; // 当前关卡
    int m_currentWordIndex;
    int m_wordCount; // 当前关卡单词数
    double m_wordDisplayTime; //当前关卡时间

    QTimer * m_countdownTimer;  // 倒计时计时器
    double m_remainingTime;    //  停止倒计时

    std::vector<QString> m_words; // 当前关卡的单词列表


    int getWordCountForCurrentStage() const; // 获取当前关卡所需单词数目


    std::pair<int, int> getMinMaxWordLength() const;

    QVector<LeaderboardEntry> m_leaderboardData;


};

#endif // GAME_H
