/********************************************************************************
** Form generated from reading UI file 'navigation.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NAVIGATION_H
#define UI_NAVIGATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Navigation
{
public:
    QWidget *centralwidget;
    QLabel *congest_label;

    void setupUi(QMainWindow *Navigation)
    {
        if (Navigation->objectName().isEmpty())
            Navigation->setObjectName("Navigation");
        Navigation->resize(550, 770);
        Navigation->setMinimumSize(QSize(550, 770));
        Navigation->setMaximumSize(QSize(550, 770));
        centralwidget = new QWidget(Navigation);
        centralwidget->setObjectName("centralwidget");
        congest_label = new QLabel(centralwidget);
        congest_label->setObjectName("congest_label");
        congest_label->setGeometry(QRect(10, 730, 381, 41));
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\271\274\345\234\206")});
        font.setPointSize(15);
        congest_label->setFont(font);
        Navigation->setCentralWidget(centralwidget);

        retranslateUi(Navigation);

        QMetaObject::connectSlotsByName(Navigation);
    } // setupUi

    void retranslateUi(QMainWindow *Navigation)
    {
        Navigation->setWindowTitle(QCoreApplication::translate("Navigation", "\345\257\274\350\210\252", nullptr));
        congest_label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Navigation: public Ui_Navigation {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NAVIGATION_H
