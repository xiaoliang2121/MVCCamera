#include "gammavalset.h"
#include "ui_gammavalset.h"

GammaValSet::GammaValSet(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GammaValSet)
{
    ui->setupUi(this);
}

GammaValSet::~GammaValSet()
{
    delete ui;
}
