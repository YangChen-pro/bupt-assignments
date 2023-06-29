#ifndef QUESTIONERWIDGET_H
#define QUESTIONERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDebug>
#include <QValidator>
#include <QMessageBox>
#include "client.h"

class QuestionerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QuestionerWidget(Client *client, QWidget *parent = nullptr);
    void WordIsEmpty();

public slots:
    void updateQuestionerInfo(const QJsonObject &userInfo);

private:
    Client *m_client;
    QLabel m_userLevelLabel;
    QLabel m_userExpLabel;
    QLabel m_questionerTitle;
    QLabel m_usernameLabel;
    QLabel m_hasCreateCount;
    QLineEdit m_wordInput1;
    QPushButton m_addWordButton;
    QPushButton m_backButton;
    QVBoxLayout m_layout;

    void addWordSlot();

private slots:
    void onWordAddedSuccessfully();
    void onWordAdditionFailed(const QString &message);

signals:
    void backClicked();

};

#endif // QUESTIONERWIDGET_H
