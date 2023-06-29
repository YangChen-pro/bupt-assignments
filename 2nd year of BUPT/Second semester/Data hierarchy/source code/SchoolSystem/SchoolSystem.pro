QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CLOCK_TIME.cpp \
    activity.cpp \
    activityschedule.cpp \
    activitywindow.cpp \
    adminwindow.cpp \
    alarmwindow.cpp \
    graph.cpp \
    loginwindow.cpp \
    main.cpp \
    navigate.cpp \
    navigation.cpp \
    person.cpp \
    schedule.cpp \
    schedulewindow.cpp \
    sortedtempdialog.cpp \
    sortresultdialog.cpp \
    studentwindow.cpp \
    temporaryschedule.cpp \
    temporarything.cpp \
    tempwindow.cpp

HEADERS += \
    CLOCK_TIME.h \
    activity.h \
    activityschedule.h \
    activitywindow.h \
    adminwindow.h \
    alarmwindow.h \
    graph.h \
    log.h \
    loginwindow.h \
    mymap.h \
    navigate.h \
    navigation.h \
    person.h \
    priorityQueue.h \
    schedule.h \
    schedulewindow.h \
    sortedtempdialog.h \
    sortresultdialog.h \
    studentwindow.h \
    temporaryschedule.h \
    temporarything.h \
    tempwindow.h

FORMS += \
    navigation.ui

TRANSLATIONS += \
    SchoolSystem_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


RESOURCES += \
image.qrc \
    image.qrc


DISTFILES += \
    点集 \
    点集 \
    边集

