#ifndef LEADERBOARDWIDGET_H
#define LEADERBOARDWIDGET_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include "client.h"

class LeaderboardWidget : public QWidget
{
    Q_OBJECT

public:
    struct LeaderboardEntry
    {
        int id;
        QString username;
        int level;
        int exp;
        int currentLevel;
        int maxLevel;
        int questions;
    };

    explicit LeaderboardWidget(Client *client, QWidget *parent = nullptr);
    void setData(const QJsonArray &data);

signals:
    void sortOrderChanged(int sortOrder);
    void backButtonClicked();

private:
    Client *m_client;
    void updateView();

    QVector<LeaderboardEntry> m_data;
};

#endif // LEADERBOARDWIDGET_H
