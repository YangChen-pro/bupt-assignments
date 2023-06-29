#include "questionerwidget.h"
#include <QRegularExpressionValidator>

QuestionerWidget::QuestionerWidget(Client *client, QWidget *parent)
    : QWidget(parent),
    m_client(client),
    m_userLevelLabel("等级："),
    m_userExpLabel("经验: "),
    m_questionerTitle("添加单词"),
    m_usernameLabel("用户名: "),
    m_hasCreateCount("当前已出题数量: "),
    m_wordInput1(),
    m_addWordButton("添加"),
    m_backButton("返回主界面")
{
    m_wordInput1.setPlaceholderText("输入单词");

    // 限制输入框只能输入英文字母
    QRegularExpression regExp("[a-zA-Z]+");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regExp, this);
    m_wordInput1.setValidator(validator);

    m_layout.addWidget(&m_questionerTitle);
    m_layout.addWidget(&m_usernameLabel);
    m_layout.addWidget(&m_userLevelLabel);
    m_layout.addWidget(&m_userExpLabel);
    m_layout.addWidget(&m_hasCreateCount);
    m_layout.addWidget(&m_wordInput1);
    m_layout.addWidget(&m_addWordButton);
    m_layout.addWidget(&m_backButton);
    setLayout(&m_layout);

    connect(&m_addWordButton, &QPushButton::clicked, this, &QuestionerWidget::addWordSlot);
    connect(&m_backButton, &QPushButton::clicked, this, &QuestionerWidget::backClicked);

    connect(m_client, &Client::userInfoReceived, this, &QuestionerWidget::updateQuestionerInfo);
}

void QuestionerWidget::updateQuestionerInfo(const QJsonObject &userInfo)
{
    // 更新用户信息
    m_usernameLabel.setText("用户名：" + userInfo["username"].toString());
    m_userLevelLabel.setText("等级：" + QString::number(userInfo["level"].toInt()));
    m_userExpLabel.setText("经验：" + QString::number(userInfo["exp"].toInt()));
    m_hasCreateCount.setText("当前已出题数量: " + QString::number(userInfo["createdWords"].toInt()));
}

void QuestionerWidget::WordIsEmpty()
{
    QMessageBox::warning(this, "添加单词", "添加内容不能为空");
}

void QuestionerWidget::addWordSlot()
{
    QString inputWord = m_wordInput1.text();
    if (inputWord == "")
    {
        WordIsEmpty();
        return;
    }
    m_client->sendAddWordRequest(inputWord);

    // 连接信号和槽
    connect(m_client, &Client::wordAddedSuccessfully, this, &QuestionerWidget::onWordAddedSuccessfully);
    connect(m_client, &Client::wordAdditionFailed, this, &QuestionerWidget::onWordAdditionFailed);
}

void QuestionerWidget::onWordAddedSuccessfully()
{
    QMessageBox::information(this, "添加单词", "单词添加成功！");
    m_wordInput1.clear();
    m_client->sendGetUserInfoRequest();
}

void QuestionerWidget::onWordAdditionFailed(const QString &message)
{
    QMessageBox::warning(this, "添加单词", message);
}
