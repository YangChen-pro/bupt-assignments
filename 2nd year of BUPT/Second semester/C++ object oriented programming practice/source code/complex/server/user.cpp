#include "User.h"

User::User(const QString& userId, const QString& username, const QString& password,
           int level, int exp, int passedStage, int maxStage, int createdWords)
    : m_userId(userId), m_username(username), m_password(password), m_level(level), m_exp(exp)
{
    m_passedStage = passedStage;
    m_maxStage = maxStage;
    m_createdWords = createdWords;
}

User::~User()
{
}

QString User::getUserId() const
{
    return m_userId;
}

QString User::getUsername() const
{
    return m_username;
}

QString User::getPassword() const
{
    return m_password;
}

int User::getLevel() const
{
    return m_level;
}

int User::getExp() const
{
    return m_exp;
}

void User::gainExp(int exp)
{
    if (exp >= 0)
    {
        int newExp = getExp() + exp;
        // 计算新等级,100经验升1级
        int newLevel = getLevel() + (newExp / 100);
        newExp %= 100;
        setLevel(newLevel);
        setExp(newExp);
    }
    else
    {
        int newExp = getExp() + exp;
        int newLevel = getLevel();
        while (newExp < 0)
        {
            newExp += 100;
            newLevel -= 1;
        }
        setLevel(newLevel);
        setExp(newExp);
    }
}

void User::setLevel(int level)
{
    m_level = level;
}

void User::setExp(int exp)
{
    m_exp = exp;
}

int User::getPassedStage() const
{
    return m_passedStage;
}

int User::getMaxStage() const
{
    return m_maxStage;
}

int User::getCreatedWords() const
{
    return m_createdWords;
}

void User::setPassedStage(int stage)
{
    m_passedStage = stage;
}

void User::setMaxStage(int stage)
{
    m_maxStage = stage;
}

void User::setCreatedWords(int words)
{
    m_createdWords = words;
}
