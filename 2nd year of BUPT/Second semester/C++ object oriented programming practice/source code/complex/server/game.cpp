#include "game.h"
#include <QTimer>

Game::Game()
    : m_currentUser(nullptr), m_isPlayer(true), m_currentStage(0), m_currentWordIndex(0)
{
    // 使用 std::chrono 作为随机数生成器的种子
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    m_randomGenerator.seed(seed);
    connectToDatabase();
}

Game::~Game(){
    closeDatabase();
}

bool Game::connectToDatabase()
{
    if (QSqlDatabase::contains()) {
        // 默认连接已存在，重用它
        m_db = QSqlDatabase::database();
    } else {
        // 创建新的默认连接
        m_db = QSqlDatabase::addDatabase("QODBC");
    }
    m_db.setDatabaseName(QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)};FIL={MS Access};DBQ=%1").arg("D:\\program\\qt\\complex\\account.accdb"));

    if (!m_db.open())
    {
        qDebug() << "Error: Failed to connect to database.";
        qDebug() << m_db.lastError();
        return false;
    }

    return true;
}

void Game::closeDatabase()
{
    if (m_db.isOpen())
    {
        m_db.close();
    }
}

int Game::getCurrentStage() const{
    return m_currentStage;
}

int Game::getCurrentWordCount() const{
    return m_wordCount;
}

void Game::moveToNextStage(){

    m_currentUser->setPassedStage(m_currentStage); // 更新当前已闯关卡数目
    if (m_currentStage > m_currentUser->getMaxStage())
        m_currentUser->setMaxStage(m_currentStage);
    int exp = calculateExpGain(); // 更新等级经验
    m_currentUser->gainExp(exp);
    if (!updateUserInDatabase())
        qDebug() << "Error: Failed to update user in database.";
    m_currentStage++;
    m_words.clear();
    m_currentWordIndex = 0;
    m_wordDisplayTime = getWordDisplayTime();
    m_wordCount = getWordCountForCurrentStage();
    m_words = chooseWordsFromPool(m_currentStage, m_wordCount);
}

QString Game::getWordAt(int wordIndex) const{
    return m_words[wordIndex];
}

double Game::getRemainingTime() const{
    return m_remainingTime;
}

User* Game::getCurrentUser() const
{
    return m_currentUser;
}


bool Game::login(const QString& username, const QString& password)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT 用户ID, 用户名, 密码, 等级, 经验值, 当前已闯关卡数, 最高闯关数, 出题数目 FROM users WHERE 用户名 = :username AND 密码 = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (!query.exec())
    {
        qDebug() << "Error: Failed to execute login query.";
        qDebug() << query.lastError();
        return false;
    }

    if (query.next())
    {
        m_currentUser = createUserFromQuery(query); // 使用查询结果创建用户对象
        return true; // 登录成功
        emit userLoggedIn();
    }

    return false; // 登录失败，未找到匹配的用户
}

bool Game::registerUser(const QString& username, const QString& password)
{
    // 检查用户名或密码是否为空
    if (username.isEmpty() || password.isEmpty())
    {
        qDebug() << "Error: Username or password cannot be empty.";
        return false;
    }

    // 检查数据库中是否存在相同用户名的用户
    QSqlQuery checkQuery(m_db);
    checkQuery.prepare("SELECT COUNT(*) FROM users WHERE 用户名 = :username");
    checkQuery.bindValue(":username", username);

    if (!checkQuery.exec())
    {
        qDebug() << "Error: Failed to execute check query.";
        qDebug() << checkQuery.lastError();
        return false;
    }

    checkQuery.next();
    int userCount = checkQuery.value(0).toInt();

    if (userCount > 0)
    {
        qDebug() << "Error: Username already exists.";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("INSERT INTO users (用户名, 密码, 等级, 经验值, 当前已闯关卡数, 最高闯关数, 出题数目) "
                  "VALUES (:username, :password, 1, 0, 0, 0, 0)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (!query.exec())
    {
        qDebug() << "Error: Failed to execute register query.";
        qDebug() << query.lastError();
        return false;
    }

    return true;
}

void Game::logout()
{
    if (m_currentUser)
    {
        delete m_currentUser;
        m_currentUser = nullptr;
    }
}

void Game::selectRole(bool isPlayer)
{
    m_isPlayer = isPlayer;
}

void Game::startStage(int stage)
{
    m_currentStage = stage;
    m_currentUser->setPassedStage(stage - 1);
    m_currentWordIndex = 0;
    m_wordDisplayTime = getWordDisplayTime();
    m_wordCount = getWordCountForCurrentStage();
    m_words = chooseWordsFromPool(m_currentStage, m_wordCount);
}

bool Game::submitWord(const QString& word)
{
    if (m_currentWordIndex >= (int)m_words.size())
    {
        return false;
    }

    if (word == m_words[m_currentWordIndex])
    {
        m_currentWordIndex++;
        return true;
    }

    return false;
}

bool Game::addWord(const QString& word)
{
    // 检查数据库中是否存在相同的单词
    QSqlQuery checkQuery(m_db);
    checkQuery.prepare("SELECT COUNT(*) FROM words WHERE 单词内容 = :word");
    checkQuery.bindValue(":word", word);

    if (!checkQuery.exec())
    {
        qDebug() << "Error: Failed to execute check query.";
        qDebug() << checkQuery.lastError();
        return false;
    }

    checkQuery.next();
    int wordCount = checkQuery.value(0).toInt();

    if (wordCount > 0)
    {
        qDebug() << "Error: Word already exists.";
        return false;
    }

    QString queryStr = QString("INSERT INTO words (单词内容, 单词长度, 添加者用户名) VALUES ('%1', %2, '%3')")
                           .arg(word)
                           .arg(word.length())
                           .arg(m_currentUser->getUsername());

    QSqlQuery query(m_db);
    if (!query.exec(queryStr))
    {
        qDebug() << "Error: Failed to execute addWord query.";
        qDebug() << query.lastError();
        return false;
    }

    // 更新 users 表
    int pre = m_currentUser->getCreatedWords();
    m_currentUser->setCreatedWords(pre + 1);
    m_currentUser->gainExp(100);
    if (!updateUserInDatabase())
        qDebug() << "Error: Failed to update user in database.";
    return true;
}
int Game::calculateExpGain() const
{
    int cnt = (m_currentStage - 1) / 5 + 1;
    cnt = std::min(cnt, 5);
    return cnt * 10 + m_currentStage * 10 + (int)(945 / m_wordDisplayTime);
}

double Game::getWordDisplayTime() const
{
    // 根据关卡计算单词展示时间：
    if (m_currentStage <= 5)
        return 5.0;
    else if (m_currentStage <= 10)
        return 4.5;
    else if (m_currentStage <= 15)
        return 4.0;
    else if (m_currentStage <= 20)
        return 3.5;
    else
        return 3.0;
}

int Game::getWordCountForCurrentStage() const
{
    // 根据关卡计算所需单词数目
    if (m_currentStage <= 5)
        return 1;
    else if (m_currentStage <= 10)
        return 2;
    else if (m_currentStage <= 15)
        return 3;
    else if (m_currentStage <= 20)
        return 4;
    else
        return 5;
}

std::vector<QString> Game::chooseWordsFromPool(int stage, int wordCount) const
{
    auto [minWordLength, maxWordLength] = getMinMaxWordLength();
    int minLength, maxLength;
    if (stage < minWordLength){
        minLength = minWordLength;
        maxLength = minWordLength;
    } else{
        minLength = std::max(minWordLength, stage);
        maxLength = std::min(maxWordLength, stage);
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM words WHERE 单词长度 >= :minLength AND 单词长度 <= :maxLength");
    query.bindValue(":minLength", minLength);
    query.bindValue(":maxLength", maxLength);

    if (!query.exec())
    {
        qDebug() << "Error: Failed to execute chooseWordsFromPool query.";
        qDebug() << query.lastError();
        return {};
    }

    std::vector<QString> wordPool;
    while (query.next())
    {
        wordPool.push_back(query.value("单词内容").toString());
    }

    if ((int)wordPool.size() < wordCount)
    {
        qDebug() << wordCount << "\n";
        qDebug() << "Warning: Not enough words in the pool for the current stage.";
        wordCount = wordPool.size();
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(wordPool.begin(), wordPool.end(), g);

    return std::vector<QString>(wordPool.begin(), wordPool.begin() + wordCount);
}

std::pair<int, int> Game::getMinMaxWordLength() const
{
    QSqlQuery query(m_db);
    query.prepare("SELECT MIN(单词长度) as minLength, MAX(单词长度) as maxLength FROM words");

    if (!query.exec() || !query.next())
    {
        qDebug() << "Error: Failed to execute getMinMaxWordLength query.";
        qDebug() << query.lastError();
        return {0, 0};
    }

    int minLength = query.value("minLength").toInt();
    int maxLength = query.value("maxLength").toInt();

    return {minLength, maxLength};
}

void Game::startCountdown()
{
    m_remainingTime = getWordDisplayTime();

    // 创建并启动倒计时定时器
    m_countdownTimer = new QTimer();
    m_countdownTimer->setInterval(100); // 0.1秒间隔
    QObject::connect(m_countdownTimer, &QTimer::timeout, [&]() {
        m_remainingTime -= 0.1;
        emit countdownUpdated(m_remainingTime);
        if (m_remainingTime <= 0) {
            // 停止定时器
            m_countdownTimer->stop();
        }
    });
    m_countdownTimer->start();
}

void Game::stopCountdown()
{
    if (m_countdownTimer != nullptr) {
        m_countdownTimer->stop();
        delete m_countdownTimer;
        m_countdownTimer = nullptr;
    }
}

// 从查询结果中获取用户信息
User* Game::createUserFromQuery(const QSqlQuery& query)
{

    QString userId = query.value(0).toString();
    QString username = query.value(1).toString();
    QString password = query.value(2).toString();
    int level = query.value(3).toInt();
    int exp = query.value(4).toInt();
    int passedStage = query.value(5).toInt();
    int maxStage = query.value(6).toInt();
    int createdWords = query.value(7).toInt();

    // 创建 User 对象
    return new User(userId, username, password, level, exp, passedStage, maxStage, createdWords);
}

bool Game::updateUserInDatabase() {


    QSqlQuery query(m_db);
    query.prepare("UPDATE users SET 等级=:level, 经验值=:exp, 当前已闯关卡数=:passedStage, 最高闯关数=:maxStage, 出题数目=:createdWords WHERE 用户ID=:userId");

    query.bindValue(":level", m_currentUser->getLevel());
    query.bindValue(":exp", m_currentUser->getExp());
    query.bindValue(":passedStage", m_currentUser->getPassedStage());
    query.bindValue(":maxStage", m_currentUser->getMaxStage());
    query.bindValue(":createdWords", m_currentUser->getCreatedWords());
    query.bindValue(":userId", m_currentUser->getUserId());

    if (!query.exec()) {
        qDebug() << "Error: Failed to update user data. " << query.lastError();
        return false;
    }

    return true;
}

void Game::fetchLeaderboardData()
{
    m_leaderboardData.clear();

    QSqlQuery query("SELECT * FROM users");
    while (query.next())
    {
        QSqlRecord record = query.record();
        LeaderboardEntry entry;
        entry.id = record.value("用户ID").toInt();
        entry.username = record.value("用户名").toString();
        entry.level = record.value("等级").toInt();
        entry.exp = record.value("经验值").toInt();
        entry.currentLevel = record.value("当前已闯关卡数").toInt();
        entry.maxLevel = record.value("最高闯关数").toInt();
        entry.questions = record.value("出题数目").toInt();

        m_leaderboardData.append(entry);
    }
}

void Game::sortLeaderboardData(int sortBy)
{
    auto compareByLevelExp = [](const LeaderboardEntry &a, const LeaderboardEntry &b) {
        if (a.level == b.level)
        {
            if (a.exp == b.exp)
            {
                return a.username < b.username;
            }
            return a.exp > b.exp;
        }
        return a.level > b.level;
    };

    switch (sortBy)
    {
    case 0: // 按等级经验排序
        std::sort(m_leaderboardData.begin(), m_leaderboardData.end(), compareByLevelExp);
        break;
    case 1: // 按最高闯关数排序
        std::sort(m_leaderboardData.begin(), m_leaderboardData.end(), [&](const LeaderboardEntry &a, const LeaderboardEntry &b) {
            if (a.maxLevel == b.maxLevel)
            {
                return compareByLevelExp(a, b);
            }
            return a.maxLevel > b.maxLevel;
        });
        break;
    case 2: // 按出题数目排序
        std::sort(m_leaderboardData.begin(), m_leaderboardData.end(), [&](const LeaderboardEntry &a, const LeaderboardEntry &b) {
            if (a.questions == b.questions)
            {
                return compareByLevelExp(a, b);
            }
            return a.questions > b.questions;
        });
        break;
    }
}

QJsonArray Game::getLeaderboardData() const
{
    QJsonArray leaderboardData;

    for (const auto &entry : m_leaderboardData)
    {
        QJsonObject entryObj;
        entryObj["id"] = entry.id;
        entryObj["username"] = entry.username;
        entryObj["level"] = entry.level;
        entryObj["exp"] = entry.exp;
        entryObj["currentLevel"] = entry.currentLevel;
        entryObj["maxLevel"] = entry.maxLevel;
        entryObj["questions"] = entry.questions;

        leaderboardData.append(entryObj);
    }

    return leaderboardData;
}
