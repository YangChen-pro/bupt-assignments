
#include "phase4.h"
#include "ui_phase4.h"


phase4::phase4(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::phase4)
{
    ui->setupUi(this);
}

phase4::~phase4()
{
    delete ui;
}


