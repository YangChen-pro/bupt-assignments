#ifndef PLAYERCHOICEWIDGET_H
#define PLAYERCHOICEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include "client.h"

class PlayerChoiceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerChoiceWidget(Client *client, QWidget *parent = nullptr);

public slots:
    void setLastStage(const QJsonObject &userInfo);

signals:
    void backButtonClicked();
    void startFromFirstButtonClicked();
    void startFromLastButtonClicked();


private:
    Client *m_client;
    QVBoxLayout m_layout;
    QLabel m_titleLabel;
    QLabel m_lastStageLabel;
    QPushButton m_startFromFirstButton;
    QPushButton m_startFromLastButton;
    QPushButton m_backButton;

private slots:
    void onBackButtonClicked();
    void onStartFromFirstButtonClicked();
    void onStartFromLastButtonClicked();


};

#endif // PLAYERCHOICEWIDGET_H
