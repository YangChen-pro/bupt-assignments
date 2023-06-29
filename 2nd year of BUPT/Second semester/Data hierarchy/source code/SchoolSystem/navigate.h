#ifndef NAVIGATE_H
#define NAVIGATE_H
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include "graph.h"
#include <QLineEdit>
#include <QLabel>
#include "log.h"


class NavigateWindow: public QWidget
{
    Q_OBJECT
public:
    NavigateWindow(logTXT*logtxt,QWidget* parent=nullptr);
    logTXT*logtxt;
    Graph *gh;
    int mode;

private:
    QVBoxLayout *mainLayout;

    QPushButton *mode_1;
    QPushButton*mode_2;
    QLineEdit*mode_3;
    QLabel*mode_4;
    QPushButton*qbutton_0;
    QPushButton*qbutton_1;


};

#endif // NAVIGATE_H
