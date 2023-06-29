
#ifndef SCHOOLSYSTEM_H
#define SCHOOLSYSTEM_H

#include <QMainWindow>



QT_BEGIN_NAMESPACE
namespace Ui { class SchoolSystem; }
QT_END_NAMESPACE

class SchoolSystem : public QMainWindow

{
    Q_OBJECT

public:
    SchoolSystem(QWidget *parent = nullptr);
    ~SchoolSystem();

private:
    Ui::SchoolSystem *ui;
};

#endif // SCHOOLSYSTEM_H
