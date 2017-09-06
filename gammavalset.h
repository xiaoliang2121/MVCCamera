#ifndef GAMMAVALSET_H
#define GAMMAVALSET_H

#include <QDialog>

namespace Ui {
class GammaValSet;
}

class GammaValSet : public QDialog
{
    Q_OBJECT

public:
    explicit GammaValSet(QWidget *parent = 0);
    ~GammaValSet();

private:
    Ui::GammaValSet *ui;
};

#endif // GAMMAVALSET_H
