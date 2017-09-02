#ifndef TRIGSETTINGSDLG_H
#define TRIGSETTINGSDLG_H

#include <QDialog>
#include <minwindef.h>
#include <MVCAPI.h>

namespace Ui {
class TrigSettingsDlg;
}

class TrigSettingsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TrigSettingsDlg(QWidget *parent = 0);
    ~TrigSettingsDlg();

    HANDLE m_hMVC3000;
    BOOL m_bConnect;

private slots:
    void on_checkBox_TrigUpOrDown_stateChanged(int arg1);

    void on_pushButton_SetPalse_clicked();

    void on_pushButton_DelayToAct_clicked();

private:
    Ui::TrigSettingsDlg *ui;

    BOOL m_bTrigPolarity;
    UINT m_nTriggerWidth;
    UINT m_nTrgDelay;
};

#endif // TRIGSETTINGSDLG_H
