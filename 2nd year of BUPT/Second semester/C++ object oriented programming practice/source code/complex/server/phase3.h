
#ifndef PHASE3_H
#define PHASE3_H

#include <QMainWindow>



QT_BEGIN_NAMESPACE
namespace Ui { class phase3; }
QT_END_NAMESPACE

class phase3 : public QMainWindow

{
    Q_OBJECT

public:
    phase3(QWidget *parent = nullptr);
    ~phase3();

private:
    Ui::phase3 *ui;
};

#endif // PHASE3_H
