#include "trigsettingsdlg.h"
#include "ui_trigsettingsdlg.h"
#include <QDebug>

TrigSettingsDlg::TrigSettingsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrigSettingsDlg)
{
    ui->setupUi(this);

    QIcon windowIcon = QIcon(tr(":/icon/param.ICO"));
    setWindowIcon(windowIcon);

    m_hMVC3000 = NULL;
    m_bConnect = FALSE;

    // 默认值
    m_bTrigPolarity = 1;
    m_bFlashPolarity = 1;
    m_nTriggerWidth = 10;
    m_nTrgDelay = 0;
    m_nFlashWidth = 500;
    m_nFlashDelay = 0;

    ui->lineEdit_SetPalse->setText(QString("%1").arg(m_nTriggerWidth));
    ui->lineEdit_DelayToAct->setText(QString("%1").arg(m_nTrgDelay));
    ui->lineEdit_FlashWidth->setText(QString("%1").arg(m_nFlashWidth));
    ui->lineEdit_FlashDelay->setText(QString("%1").arg(m_nFlashDelay));
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
        qDebug()<<"选中了"<<endl;
    }
    else if(Qt::Unchecked == arg1)
    {
        m_bTrigPolarity = 0;
        qDebug()<<"取消选择"<<endl;
    }
    int rt = MV_Usb2SetSignalPolarity(m_hMVC3000,m_bTrigPolarity,m_bFlashPolarity);
    if(rt == 0)
    {
     qDebug()<<"设置成功"<<endl;
    }
    else
    {
     qDebug()<<"设置失败"<<endl;
    }
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

void TrigSettingsDlg::on_pushButton_FlashWidth_clicked()
{
    if(!m_bConnect)
        return;

    m_nFlashWidth = ui->lineEdit_FlashWidth->text().toInt();
    MV_Usb2SetLedWidth(m_hMVC3000,m_nFlashWidth);
}

void TrigSettingsDlg::on_pushButton_FlashDelay_clicked()
{
    if(!m_bConnect)
        return;

    m_nFlashDelay = ui->lineEdit_FlashDelay->text().toInt();
    MV_Usb2SetLedDelay(m_hMVC3000,m_nFlashDelay);
}

void TrigSettingsDlg::on_checkBox_FlashUpOrDown_stateChanged(int arg1)
{
    if(!m_bConnect)
        return;

    if(Qt::Checked == arg1)
    {
        m_bFlashPolarity = 1;
        qDebug()<<"选中了"<<endl;
    }
    else if(Qt::Unchecked == arg1)
    {
        m_bFlashPolarity = 0;
        qDebug()<<"取消选择"<<endl;
    }
    MV_Usb2SetSignalPolarity(m_hMVC3000,m_bTrigPolarity,m_bFlashPolarity);
}

void TrigSettingsDlg::on_pushButton_Ok_clicked()
{
//    emit on_checkBox_TrigUpOrDown_stateChanged(Qt::Checked);
//    emit on_checkBox_FlashUpOrDown_stateChanged(Qt::Checked);
    emit on_pushButton_SetPalse_clicked();
    emit on_pushButton_DelayToAct_clicked();
    emit on_pushButton_FlashWidth_clicked();
    emit on_pushButton_FlashDelay_clicked();

    // 曝光时间设置
    int exposure = ui->lineEdit_SetExposure->text().toInt();
    m_CapInfo.Exposure = exposure;
    MV_Usb2SetPartOfCapInfo(m_hMVC3000,&m_CapInfo);
    emit updateExposure(exposure);

    close();
}
