#ifndef TODOWINDOW_H
#define TODOWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDateTime>

class ToDoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ToDoWindow(const QString &name, const QString &studentId, QWidget *parent = nullptr);
    ~ToDoWindow();

private:
    QLabel *nameLabel;
    QLabel *studentIdLabel;
    QListWidget *menuListWidget;
    QStackedWidget *stackedWidget;
    QHBoxLayout *topLayout;
    QVBoxLayout *mainLayout;
};

#endif // TODOWINDOW_H
