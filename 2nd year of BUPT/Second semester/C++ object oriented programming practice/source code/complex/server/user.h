#ifndef USER_H
#define USER_H

#include <QString>

class User
{
public:
    User(const QString& userId, const QString& username, const QString& password,
         int level, int exp, int passedStage, int maxStage, int createdWords);
    ~User();

    QString getUserId() const;
    QString getUsername() const;
    QString getPassword() const;
    int getLevel() const;
    int getExp() const;
    int getPassedStage() const;
    int getMaxStage() const;
    int getCreatedWords() const;

    void setLevel(int level);
    void setExp(int exp);
    void setPassedStage(int stage);
    void setMaxStage(int stage);
    void setCreatedWords(int words);
    void gainExp(int exp);

private:
    QString m_userId;
    QString m_username;
    QString m_password;
    int m_level;
    int m_exp;
    int m_passedStage;
    int m_maxStage;
    int m_createdWords;
};


#endif // USER_H
