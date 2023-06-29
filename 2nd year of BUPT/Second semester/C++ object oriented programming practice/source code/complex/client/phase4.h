
#ifndef PHASE4_H
#define PHASE4_H

#include <QMainWindow>



QT_BEGIN_NAMESPACE
namespace Ui { class phase4; }
QT_END_NAMESPACE

class phase4 : public QMainWindow

{
    Q_OBJECT

public:
    phase4(QWidget *parent = nullptr);
    ~phase4();

private:
    Ui::phase4 *ui;
};

#endif // PHASE4_H
