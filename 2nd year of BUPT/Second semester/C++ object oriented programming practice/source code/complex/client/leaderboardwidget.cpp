#include "leaderboardwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QComboBox>

LeaderboardWidget::LeaderboardWidget(Client *client, QWidget *parent)
    : QWidget(parent)
{
    this->m_client = client;
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 添加标题
    QLabel *titleLabel = new QLabel("排行榜", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    // 添加表格
    QTableWidget *tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(7);
    tableWidget->setHorizontalHeaderLabels({"ID", "用户名", "等级", "经验", "当前关卡", "最高关卡", "出题数"});
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);
    tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    tableWidget->setSelectionMode(QTableWidget::SingleSelection);
    tableWidget->setAlternatingRowColors(true);
    layout->addWidget(tableWidget);

    // 添加排序下拉菜单
    QHBoxLayout *sortMenuLayout = new QHBoxLayout();
    QLabel *sortOrderLabel = new QLabel("排序方式：", this);
    QComboBox *sortOrderComboBox = new QComboBox(this);
    sortOrderComboBox->addItem("按等级和经验排序");
    sortOrderComboBox->addItem("按最高关卡排序");
    sortOrderComboBox->addItem("按出题数排序");
    sortMenuLayout->addWidget(sortOrderLabel);
    sortMenuLayout->addWidget(sortOrderComboBox);
    layout->addLayout(sortMenuLayout);

    // 添加返回按钮
    QPushButton *backButton = new QPushButton("返回", this);
    layout->addWidget(backButton);

    // 信号槽连接
    connect(sortOrderComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LeaderboardWidget::sortOrderChanged);
    connect(backButton, &QPushButton::clicked, this, &LeaderboardWidget::backButtonClicked);
    connect(client, &Client::sortedData, this, &LeaderboardWidget::setData);
}

void LeaderboardWidget::setData(const QJsonArray &data)
{
    m_data.clear();
    for (const auto &entryObj : data)
    {
        QJsonObject entryJson = entryObj.toObject();
        LeaderboardEntry entry;
        entry.id = entryJson["id"].toInt();
        entry.username = entryJson["username"].toString();
        entry.level = entryJson["level"].toInt();
        entry.exp = entryJson["exp"].toInt();
        entry.currentLevel = entryJson["currentLevel"].toInt();
        entry.maxLevel = entryJson["maxLevel"].toInt();
        entry.questions = entryJson["questions"].toInt();

        m_data.append(entry);
    }

    updateView();
}

void LeaderboardWidget::updateView()
{
    QTableWidget *tableWidget = findChild<QTableWidget *>();

    if (!tableWidget)
    {
        qWarning() << "未找到表格控件!";
        return;
    }

    tableWidget->setRowCount(m_data.size());

    for (int row = 0; row < m_data.size(); ++row)
    {
        const auto &entry = m_data[row];

        tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(entry.id)));
        tableWidget->setItem(row, 1, new QTableWidgetItem(entry.username));
        tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(entry.level)));
        tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(entry.exp)));
        tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(entry.currentLevel)));
        tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(entry.maxLevel)));
        tableWidget->setItem(row, 6, new QTableWidgetItem(QString::number(entry.questions)));
    }

    tableWidget->resizeColumnsToContents();
    tableWidget->resizeRowsToContents();
}
