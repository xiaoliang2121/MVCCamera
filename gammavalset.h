#ifndef GAMMAVALSET_H
#define GAMMAVALSET_H

#include <QDialog>
#include <windows.h>

namespace Ui {
class GammaValSet;
}

class GammaValSet : public QDialog
{
    Q_OBJECT

public:
    explicit GammaValSet(QWidget *parent = 0);
    ~GammaValSet();

public:
    float fValue;
    HANDLE m_hMVC3000;

private slots:
    void on_horizontalSlider_GammaValue_valueChanged(int value);
    void on_PushButton_Ok_clicked();

private:
    Ui::GammaValSet *ui;
};

#endif // GAMMAVALSET_H
