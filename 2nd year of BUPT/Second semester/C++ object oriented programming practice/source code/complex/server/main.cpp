#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include "server.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget *widget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;
    widget->setLayout(layout);

    // 添加端口号信息
    QLabel *portLabel = new QLabel("端口号:");
    QLineEdit *portEdit = new QLineEdit;
    portEdit->setText("12345"); // 设置默认端口号
    portEdit->setEnabled(false);
    layout->addWidget(portLabel);
    layout->addWidget(portEdit);

    // 添加监听地址信息
    QLabel *addressLabel = new QLabel("监听地址:");
    QLineEdit *addressEdit = new QLineEdit;
    addressEdit->setText("127.0.0.1"); // 设置默认监听地址
    addressEdit->setEnabled(false);
    layout->addWidget(addressLabel);
    layout->addWidget(addressEdit);

    // 添加启动服务器按钮
    QPushButton *button1 = new QPushButton("启动服务器");
    QPushButton *button2 = new QPushButton("停止服务器");
    layout->addWidget(button1);
    layout->addWidget(button2);

    Server server;

    QObject::connect(button1, &QPushButton::clicked, [&](){
        int port = portEdit->text().toInt(); // 获取用户输入的端口号
        QString address = addressEdit->text(); // 获取用户输入的监听地址
        if (!server.start(port))
        {
            qCritical() << "无法启动服务器";
        }
    });

    QObject::connect(button2, &QPushButton::clicked, [&](){
        server.stop();
        qDebug() << "服务器已停止";
    });

    widget->show();
    return app.exec();
}
