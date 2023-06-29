#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QFormLayout>
#include "CLOCK_TIME.h"
#include "log.h"

class AdminWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AdminWindow(logTXT* logtxt,QWidget *parent = nullptr);
    ~AdminWindow();

    logTXT *logtxt;

private:
    QLabel *adminLabel;
    QListWidget *menuListWidget;
    QStackedWidget *stackedWidget;
    QVBoxLayout *leftLayout;
    QWidget *addStudentWidget;
    QLineEdit *idLineEdit;
    QLineEdit *passwordLineEdit;
    QLineEdit *nameLineEdit;
    QTableWidget *studentTableWidget;

    void showAllStudents();

signals:
    void showAllStudentsSignal();

private slots:
    void addStudent();
    void menuItemChanged(int index);
    void deleteStudent();
    void modifyStudent();
};

#endif // ADMINWINDOW_H
