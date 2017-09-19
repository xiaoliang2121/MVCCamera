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
    CapInfoStruct m_CapInfo;        // 视频属性

public slots:
    void on_checkBox_TrigUpOrDown_stateChanged(int arg1);
    void on_checkBox_FlashUpOrDown_stateChanged(int arg1);
    void on_pushButton_SetPalse_clicked();
    void on_pushButton_DelayToAct_clicked();
    void on_pushButton_FlashWidth_clicked();
    void on_pushButton_FlashDelay_clicked();
    void on_pushButton_Ok_clicked();

signals:
    void updateExposure(ulong exp);

private:
    Ui::TrigSettingsDlg *ui;

    BOOL m_bTrigPolarity;
    BOOL m_bFlashPolarity;
    UINT m_nTriggerWidth;
    UINT m_nTrgDelay;
    UINT m_nFlashWidth;
    UINT m_nFlashDelay;
};

#endif // TRIGSETTINGSDLG_H
