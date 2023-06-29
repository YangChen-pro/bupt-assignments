#ifndef ROLESELECTIONWIDGET_H
#define ROLESELECTIONWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class RoleSelectionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RoleSelectionWidget(QWidget *parent = nullptr);

signals:
    void backButtonClicked();
    void playerButtonClicked();
    void questionerButtonClicked();

private:
    QVBoxLayout m_layout;
    QLabel m_titleLabel;
    QPushButton m_playerButton;
    QPushButton m_questionerButton;
    QPushButton m_backButton;

private slots:
    void onBackButtonClicked();
    void onPlayerButtonClicked();
    void onQuestionerButtonClicked();
};

#endif // ROLESELECTIONWIDGET_H
