QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    client.cpp \
    gamewidget.cpp \
    leaderboardwidget.cpp \
    loginwidget.cpp \
    main.cpp \
    mainwidget.cpp \
    matchmakingwidget.cpp \
    onlinebattlewidget.cpp \
    phase4.cpp \
    playerchoicewidget.cpp \
    questionerwidget.cpp \
    roleselectionwidget.cpp

HEADERS += \
    client.h \
    gamewidget.h \
    leaderboardwidget.h \
    loginwidget.h \
    mainwidget.h \
    matchmakingwidget.h \
    onlinebattlewidget.h \
    phase4.h \
    playerchoicewidget.h \
    questionerwidget.h \
    roleselectionwidget.h

FORMS += \
    phase4.ui

TRANSLATIONS += \
    phase4_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
