#include <QApplication>
#include "client.h"
#include "loginwidget.h"
#include "mainwidget.h"
#include "roleselectionwidget.h"
#include "playerchoicewidget.h"
#include "questionerwidget.h"
#include "gamewidget.h"
#include "leaderboardwidget.h"
#include "matchmakingwidget.h"
#include "onlinebattlewidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Client client;
    // 连接到服务器，这里需要提供服务器的主机名和端口
    client.connectToServer("127.0.0.1", 12345);

    LoginWidget loginWidget(&client);
    MainWidget mainWidget(&client);
    RoleSelectionWidget roleSelectionWidget;
    PlayerChoiceWidget playerChoiceWidget(&client);
    QuestionerWidget questionerWidget(&client);
    GameWidget gameWidget(&client);
    LeaderboardWidget leaderboardWidget(&client);
    MatchmakingWidget matchWidget(&client);
    OnlineBattleWidget onlineBattleWidget(&client);

    // 登录成功后进入主界面
    QObject::connect(&loginWidget, &LoginWidget::loginSuccess, [&](){
        loginWidget.hide();
        client.sendGetUserInfoRequest();
        mainWidget.show();
    });

    // 退出登录后返回登录注册界面
    QObject::connect(&mainWidget, &MainWidget::logout, [&](){
        mainWidget.hide();
        loginWidget.show();
    });

    // 在线游戏界面
    QObject::connect(&mainWidget, &MainWidget::onlineGame, [&](){
        mainWidget.hide();
        client.sendGetOnlinePlayersRequest();
        matchWidget.show();
    });

    // 在线游戏返回主界面
    QObject::connect(&matchWidget, &MatchmakingWidget::backButtonClicked, [&](){
        matchWidget.hide();
        mainWidget.show();
    });

    // 在线对战界面
    QObject::connect(&matchWidget, &MatchmakingWidget::enterBattle, [&](){
        matchWidget.hide();
        onlineBattleWidget.show();
    });

    // 对战界面返回主界面
    QObject::connect(&onlineBattleWidget, &OnlineBattleWidget::backButtonClicked, [&](){
        onlineBattleWidget.hide();
        matchWidget.playing = false;
        mainWidget.show();
    });

    // 开始游戏按钮点击事件
    QObject::connect(&mainWidget, &MainWidget::startGame, [&]() {
        mainWidget.hide();
        roleSelectionWidget.show();
    });

    // 排行榜按钮点击
    QObject::connect(&mainWidget, &MainWidget::board, [&](){
        mainWidget.hide();
        client.sendSortLeaderboardRequest(0);
        leaderboardWidget.show();
    });

    // 下拉菜单，不同排序标准
    QObject::connect(&leaderboardWidget, &LeaderboardWidget::sortOrderChanged, [&](int sortOrder) {
        client.sendSortLeaderboardRequest(sortOrder);
    });

    // 排行榜返回按钮
    QObject::connect(&leaderboardWidget, &LeaderboardWidget::backButtonClicked, [&](){
        leaderboardWidget.hide();
        mainWidget.show();
    });

    // 返回按钮
    QObject::connect(&roleSelectionWidget, &RoleSelectionWidget::backButtonClicked, [&](){
        roleSelectionWidget.hide();
        mainWidget.show();
    });

    // 在身份选择界面的闯关者按钮点击事件中显示闯关者选择界面
    QObject::connect(&roleSelectionWidget, &RoleSelectionWidget::playerButtonClicked, [&]() {
        roleSelectionWidget.hide();
        client.sendGetUserInfoRequest();
        playerChoiceWidget.show();
    });

    // 返回按钮
    QObject::connect(&playerChoiceWidget, &PlayerChoiceWidget::backButtonClicked, [&]() {
        playerChoiceWidget.hide();
        roleSelectionWidget.show();
    });

    // 在身份选择界面的点击出题者按钮
    QObject::connect(&roleSelectionWidget, &RoleSelectionWidget::questionerButtonClicked, [&](){
        roleSelectionWidget.hide();
        questionerWidget.show();
    });

    // 返回主界面
    QObject::connect(&questionerWidget, &QuestionerWidget::backClicked, [&](){
        questionerWidget.hide();
        client.sendGetUserInfoRequest();
        mainWidget.show();
    });

    // 从第一关开始
    QObject::connect(&playerChoiceWidget, &PlayerChoiceWidget::startFromFirstButtonClicked, [&]() {
        playerChoiceWidget.hide();
        gameWidget.startGame(1); // 调用 GameWidget 中的 startGame 方法
        gameWidget.show();
    });

    // 从上次闯关的位置开始
    QObject::connect(&playerChoiceWidget, &PlayerChoiceWidget::startFromLastButtonClicked, [&]() {
        playerChoiceWidget.hide();
        gameWidget.startFromLastStage(); // 调用 GameWidget 中的 startGame 方法
        gameWidget.show();
    });

    // 返回主界面
    QObject::connect(&gameWidget, &GameWidget::backClicked, [&](){
        gameWidget.startFlag = 0;
        gameWidget.hide();
        client.sendGetUserInfoRequest();
        mainWidget.show();
    });


    loginWidget.show();

    return app.exec();
}
