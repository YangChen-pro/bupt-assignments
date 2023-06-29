#include "roleselectionwidget.h"

RoleSelectionWidget::RoleSelectionWidget(QWidget *parent) :
    QWidget(parent),
    m_titleLabel("选择身份"),
    m_playerButton("闯关者"),
    m_questionerButton("出题者"),
    m_backButton("返回")
{
    m_layout.addWidget(&m_titleLabel);
    m_layout.addWidget(&m_playerButton);
    m_layout.addWidget(&m_questionerButton);
    m_layout.addWidget(&m_backButton);
    setLayout(&m_layout);

    connect(&m_backButton, &QPushButton::clicked, this, &RoleSelectionWidget::onBackButtonClicked);
    connect(&m_playerButton, &QPushButton::clicked, this, &RoleSelectionWidget::onPlayerButtonClicked);
    connect(&m_questionerButton, &QPushButton::clicked, this, &RoleSelectionWidget::onQuestionerButtonClicked);

}

void RoleSelectionWidget::onBackButtonClicked()
{
    emit backButtonClicked();
}

void RoleSelectionWidget::onPlayerButtonClicked()
{
    emit playerButtonClicked();
}

void RoleSelectionWidget::onQuestionerButtonClicked()
{
    emit questionerButtonClicked();
}
