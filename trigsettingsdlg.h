#ifndef TRIGSETTINGSDLG_H
#define TRIGSETTINGSDLG_H

#include <QDialog>

namespace Ui {
class TrigSettingsDlg;
}

class TrigSettingsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TrigSettingsDlg(QWidget *parent = 0);
    ~TrigSettingsDlg();

private:
    Ui::TrigSettingsDlg *ui;
};

#endif // TRIGSETTINGSDLG_H
