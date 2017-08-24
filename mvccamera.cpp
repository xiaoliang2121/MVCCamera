#include "mvccamera.h"
#include "ui_mvccamera.h"
#include <QMessageBox>
#include <QCloseEvent>
#include "aboutdlg.h"

MVCCamera::MVCCamera(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MVCCamera)
{
    ui->setupUi(this);
    m_hMVC3000 = NULL;

    connectAction = new QAction(tr("连接"),this);
    connectAction->setStatusTip("通过USB2.0连接到相机");
    connect(connectAction,&QAction::triggered,this,\
            &MVCCamera::onConnectActionTriggered);

    aboutAction = new QAction(tr("关于MVC_CAMERA程序"));
    aboutAction->setStatusTip("程序相关");
    connect(aboutAction,&QAction::triggered,\
            this,&MVCCamera::onShowAboutDlg);

    QMenu *connectMenu = menuBar()->addMenu(tr("USB2.0连接"));
    connectMenu->addAction(connectAction);
    QMenu *helpMenu = menuBar()->addMenu("帮助");
    helpMenu->addAction(aboutAction);

}

MVCCamera::~MVCCamera()
{
    delete ui;
    delete connectAction;
//    delete exitAction;

    delete startCapAction;
    delete pauseCapAction;
    delete stopCapAction;
}

void MVCCamera::setNewMenu()
{
    menuBar()->clear();

    // 相机开始、暂停、停止操作
    startCapAction = new QAction(tr("启动"),this);
    startCapAction->setStatusTip("启动相机，开始采集或者预览");
    connect(startCapAction,&QAction::triggered,\
            this,&MVCCamera::onStartCapActionTriggered);

    pauseCapAction = new QAction(tr("暂停"),this);
    pauseCapAction->setStatusTip("暂停");
    connect(pauseCapAction,&QAction::triggered,\
            this,&MVCCamera::onPauseCapActionTriggered);

    stopCapAction = new QAction(tr("停止"),this);
    stopCapAction->setStatusTip("停止相机，以及一些收尾工作");
    connect(stopCapAction,&QAction::triggered,\
            this,&MVCCamera::onStopCapActionTriggered);

    exitAction = new QAction(tr("退出程序"),this);\
    exitAction->setStatusTip("退出本程序");
    connect(exitAction,&QAction::triggered,\
            this,&MVCCamera::close);

    QMenu *operation = menuBar()->addMenu(tr("操作"));
    operation->addAction(startCapAction);
    operation->addAction(pauseCapAction);
    operation->addAction(stopCapAction);
    operation->addSeparator();
    operation->addAction(exitAction);

}

void MVCCamera::onConnectActionTriggered()
{
    // 设置新菜单
    setNewMenu();

    // 进行相机连接的初始化操作
    int nIndex = 0;
    int rt = MV_Usb2Init("MVC-F",&nIndex,&m_CapInfo,&m_hMVC3000);
    if(ResSuccess != rt){
        QMessageBox msgBox;
        msgBox.setText("错误：无法初始化USB相机！");
        msgBox.exec();
        MV_Usb2Uninit(&m_hMVC3000);
        m_hMVC3000 = NULL;
        return;
    }

//    MV_Usb2SetOpMode(m_hMVC3000,m_nOpMode,FALSE);
//    MV_Usb2SetRawCallBack(m_hMVC3000,RawCallBack,this);
//    MV_Usb2SetFrameCallBack(m_hMVC3000,FrameCallBack,this);
//        setText("USB相机初始化成功");

}

void MVCCamera::onShowAboutDlg()
{
    AboutDlg aboutDlg;
    aboutDlg.exec();
}

void MVCCamera::onStartCapActionTriggered()
{

}

void MVCCamera::onPauseCapActionTriggered()
{

}

void MVCCamera::onStopCapActionTriggered()
{

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
