#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include "client.h"

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(Client *client, QWidget *parent = nullptr);
    void startGame(int stage);
    void startFromLastStage();
    bool startFlag = 0; // 是否开始游戏的标志

public slots:

    void updatePlayerInfo(const QJsonObject &userInfo);
    void generateWord(const QString& newWord);
    void getRemainingTime(int timeLeft);
    void wordIsAccept();
    void wordIsWrong(const QString &message);
    void moveToNext();
    void onStartStage();

private:

    bool nextFlag; // 是否进入下一关标志
    int currentStage;
    int lastStage;
    int wordsRequired;
    int newLevel;
    int newExp;
    Client *m_client;
    QLabel *playernameLabel;
    QLabel *playerLevelLabel;
    QLabel *playerExpLabel;
    QLabel *levelLabel;
    QLabel *wordLabel;
    QLabel *wordCountLabel;
    QLabel *countdownLabel;
    QLineEdit *wordInput;
    QPushButton *submitButton;
    QPushButton *backButton;
    QVBoxLayout *gameLayout;
    QTimer *timer;
    int timeLeft;
    int wordCount = 0; // 当前成功了几个单词
    int wordIndex = 0;
    void setupUi();
    void connectSignalsAndSlots();

    void updateTimer();
    void backSlot();
    void submitSlot();



signals:
    void backClicked();
};

#endif // GAMEWIDGET_H
