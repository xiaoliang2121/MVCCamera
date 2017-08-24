#include "mvccamera.h"
#include "ui_mvccamera.h"
#include <QMessageBox>
#include <QCloseEvent>

MVCCamera::MVCCamera(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MVCCamera)
{
    ui->setupUi(this);
    m_hMVC3000 = NULL;

    connectAction = new QAction(tr("连接"),this);
    connectAction->setStatusTip("通过USB2.0连接到相机");
    connect(connectAction,&QAction::triggered,this,&MVCCamera::onConnectActionTriggered);

    aboutAction = new QAction(tr("关于MVC_CAMERA程序"));
    aboutAction->setStatusTip("相关内容");


    auto connectMenu = menuBar()->addMenu(tr("USB2.0连接"));
    connectMenu->addAction(connectAction);
}

MVCCamera::~MVCCamera()
{
    delete ui;
    delete connectAction;
    delete exitAction;
}

void MVCCamera::setNewMenu()
{
    menuBar()->clear();


}

void MVCCamera::onConnectActionTriggered()
{
    // 设置新菜单
    menuBar()->clear();

    exitAction = new QAction(tr("退出"),this);
    exitAction->setStatusTip("退出本程序");
    connect(exitAction,&QAction::triggered,this,&MVCCamera::close);

    QMenu *operation = menuBar()->addMenu(tr("操作"));
    operation->addAction(exitAction);

    // 进行相机连接的初始化操作
    int nIndex = 0;
    int rt = MV_Usb2Init("MVC-F",&nIndex,&m_CapInfo,&m_hMVC3000);
    if(ResSuccess != rt){
        QMessageBox msgBox;
        msgBox.setText("错误：无法初始化USB相机！");
        msgBox.exec();
        MV_Usb2Uninit(&m_hMVC3000);
        m_hMVC1000 = NULL;
        return;
    }

    MV_Usb2SetOpMode(m_hMVC1000,m_nOpMode,FALSE);
    MV_Usb2SetRawCallBack(m_hMVC1000,RawCallBack,this);
    MV_Usb2SetFrameCallBack(m_hMVC1000,FrameCallBack,this);
    ui->Msg_Label->setText("USB camera init successfully");

}

void MVCCamera::closeEvent(QCloseEvent *event)
{
    QMessageBox msgBox(QMessageBox::Warning,"提示","确定要关闭吗",\
                       QMessageBox::Yes|QMessageBox::No,this);
    if(msgBox.exec() == QMessageBox::Yes)
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}
