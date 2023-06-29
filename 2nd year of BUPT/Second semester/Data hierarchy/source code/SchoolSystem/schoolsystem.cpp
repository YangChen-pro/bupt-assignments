
#include "schoolsystem.h"
#include "ui_schoolsystem.h"


SchoolSystem::SchoolSystem(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SchoolSystem)
{
    ui->setupUi(this);
}

SchoolSystem::~SchoolSystem()
{
    delete ui;
}


