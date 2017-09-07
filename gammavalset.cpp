#include "gammavalset.h"
#include "ui_gammavalset.h"
#include "MVCAPI.h"
#include <QMessageBox>

GammaValSet::GammaValSet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GammaValSet)
{
    ui->setupUi(this);

    ui->horizontalSlider_GammaValue->setRange(0,200);
    ui->horizontalSlider_GammaValue->setValue((int)(fValue*100));
    ui->horizontalSlider_GammaValue->setTickInterval(10);

    QString str = QString("%1").arg(fValue);
    setWindowTitle(str);

    connect(ui->pushButton_Ok,&QPushButton::clicked,\
            this,&GammaValSet::on_PushButton_Ok_clicked);
}

GammaValSet::~GammaValSet()
{
    delete ui;
}

void GammaValSet::on_horizontalSlider_GammaValue_valueChanged(int value)
{
    float fval = value/100.0;
    QString strText = QString("%1").arg(fval);
    setWindowTitle(strText);
    int rt = MV_Usb2SetGammaValue(m_hMVC3000,fval,TRUE);
    if(ResSuccess != rt)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("设置Gamma值失败");
        msgBox.setWindowTitle("提示");
        msgBox.exec();
    }
}

void GammaValSet::on_PushButton_Ok_clicked()
{
    close();
}
