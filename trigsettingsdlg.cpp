#include "trigsettingsdlg.h"
#include "ui_trigsettingsdlg.h"

TrigSettingsDlg::TrigSettingsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrigSettingsDlg)
{
    ui->setupUi(this);

    QIcon windowIcon = QIcon(tr(":/icon/param.ICO"));
    setWindowIcon(windowIcon);

    m_bTrigPolarity = 1;
    m_hMVC3000 = NULL;
    m_bConnect = FALSE;
    m_nTriggerWidth = 1000;
    m_nTrgDelay = 0;
}

TrigSettingsDlg::~TrigSettingsDlg()
{
    delete ui;
}

void TrigSettingsDlg::on_checkBox_TrigUpOrDown_stateChanged(int arg1)
{
    if(!m_bConnect)
        return;

    if(Qt::Checked == arg1)
    {
        m_bTrigPolarity = 1;
    }
    else if(Qt::Unchecked == arg1)
    {
        m_bTrigPolarity = 0;
    }
     MV_Usb2SetSignalPolarity(m_hMVC3000,m_bTrigPolarity,0);
}

void TrigSettingsDlg::on_pushButton_SetPalse_clicked()
{
    if(!m_bConnect)
        return;

    m_nTriggerWidth = ui->lineEdit_SetPalse->text().toInt();
    MV_Usb2SetTriggerPulse(m_hMVC3000,m_nTriggerWidth);
}

void TrigSettingsDlg::on_pushButton_DelayToAct_clicked()
{
    if(!m_bConnect)
        return;

    m_nTrgDelay = ui->lineEdit_DelayToAct->text().toInt();
    MV_Usb2SetTriggerDelay(m_hMVC3000,m_nTrgDelay);
}
