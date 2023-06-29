
#include "phase3.h"
#include "ui_phase3.h"


phase3::phase3(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::phase3)
{
    ui->setupUi(this);
}

phase3::~phase3()
{
    delete ui;
}


