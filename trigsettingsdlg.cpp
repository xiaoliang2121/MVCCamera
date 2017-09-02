#include "trigsettingsdlg.h"
#include "ui_trigsettingsdlg.h"

TrigSettingsDlg::TrigSettingsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrigSettingsDlg)
{
    ui->setupUi(this);
}

TrigSettingsDlg::~TrigSettingsDlg()
{
    delete ui;
}
