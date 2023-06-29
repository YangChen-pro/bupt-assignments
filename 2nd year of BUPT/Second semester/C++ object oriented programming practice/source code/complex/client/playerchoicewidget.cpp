#include "playerchoicewidget.h"

PlayerChoiceWidget::PlayerChoiceWidget(Client *client, QWidget *parent) :
    QWidget(parent),
    m_client(client),
    m_titleLabel("请选择闯关方式"),
    m_startFromFirstButton("从第一关开始闯"),
    m_startFromLastButton("从上次的闯关位置继续闯关"),
    m_backButton("返回")
{
    m_layout.addWidget(&m_titleLabel);
    m_layout.addWidget(&m_lastStageLabel);
    m_layout.addWidget(&m_startFromFirstButton);
    m_layout.addWidget(&m_startFromLastButton);
    m_layout.addWidget(&m_backButton);
    setLayout(&m_layout);

    connect(&m_backButton, &QPushButton::clicked, this, &PlayerChoiceWidget::onBackButtonClicked);
    connect(&m_startFromFirstButton, &QPushButton::clicked, this, &PlayerChoiceWidget::onStartFromFirstButtonClicked);
    connect(&m_startFromLastButton, &QPushButton::clicked, this, &PlayerChoiceWidget::onStartFromLastButtonClicked);

    connect(m_client, &Client::userInfoReceived, this, &PlayerChoiceWidget::setLastStage);
}

void PlayerChoiceWidget::onBackButtonClicked()
{
    emit backButtonClicked();
}

void PlayerChoiceWidget::onStartFromFirstButtonClicked()
{
    emit startFromFirstButtonClicked();
}

void PlayerChoiceWidget::onStartFromLastButtonClicked()
{
    emit startFromLastButtonClicked();
}

void PlayerChoiceWidget::setLastStage(const QJsonObject &userInfo)
{
    m_lastStageLabel.setText("上次闯关位置: " + QString::number(userInfo["lastStage"].toInt()));
}
