#include "mvccamera.h"
#include "ui_mvccamera.h"
#include <QMessageBox>
#include <QCloseEvent>
#include "aboutdlg.h"

static LONG gGains;
static LONG gExposure;

MVCCamera::MVCCamera(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MVCCamera)
{
    ui->setupUi(this);

    // 参数初始化
    m_hMVC3000 = NULL;
    m_nOpMode = 0;
    m_bConnect = FALSE;
    // MVC3000F相机
    MAXWIDTH=2048;
    MAXHEIGHT=1536;
    m_nBrightness= 0;
    m_nContrast	 = 20;
    m_nSaturation= 100;
    m_bPreview = FALSE;
    m_bPause   = FALSE;
    m_bBw = FALSE;

    // 内存分配
    DWORD RGBDataSize = MAXWIDTH*MAXHEIGHT*3;
    m_pRGBData = (BYTE*)malloc(RGBDataSize*sizeof(BYTE));
    memset(m_pRGBData,0,RGBDataSize);
    DWORD RawDataSize = MAXWIDTH*MAXHEIGHT;
    m_pRawData=(BYTE*)malloc(RawDataSize*sizeof(BYTE));
    memset(m_pRawData,0,RawDataSize);
    m_CapInfo.Buffer = m_pRawData;


    // 顶层菜单设置
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
    delete aboutAction;

    delete startCapAction;
    delete pauseCapAction;
    delete stopCapAction;
    delete exitAction;
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
    pauseCapAction->setStatusTip("暂停预览");
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

void AWBFunction(LPVOID pParam)
{
    // 这里使用线程来处理
    Q_UNUSED(pParam);
}

void CALLBACK AEFunction(LPVOID pParam)
{
    // 这里使用线程来处理
    Q_UNUSED(pParam);
}

void CALLBACK RawCallBack(LPVOID lpParam, LPVOID lpUser)
{
    Q_UNUSED(lpParam);
    Q_UNUSED(lpUser);
}

void CALLBACK FrameCallBack(LPVOID lpParam, LPVOID lpUser)
{
    Q_UNUSED(lpParam);
    Q_UNUSED(lpUser);
}

void MVCCamera::onConnectActionTriggered()
{
    // 设置新菜单
    setNewMenu();

    // 进行相机连接的初始化操作
    if(m_bConnect)
        return;

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

    // 显示设备数(先处理只有一个相机的情况)
    MV_Usb2GetNumberDevices(m_hMVC3000,&m_nDeviceNum);
//    LPTSTR temp;
//    MV_Usb2GetSerial(m_hMVC3000,temp);
//    QString str;
//    str.fromLatin1(temp);
//    m_strDeviceNum = QString("MVC3000F Num:%1, S/N:%2").arg(m_nDeviceNum).arg(str);

    m_bConnect = TRUE;

//    MV_Usb2SetOpMode(m_hMVC3000,m_nOpMode,FALSE);

    // 设置回调函数
    MV_Usb2SetAwbCallBackFunction(m_hMVC3000,180,180,180,reinterpret_cast<LPVOID>(AWBFunction),&gGains);  // 自动白平衡
    MV_Usb2SetAeCallBackFunction(m_hMVC3000,180,reinterpret_cast<LPVOID>(AEFunction),&gExposure);         // 自动曝光操作

    MV_Usb2SetRawCallBack(m_hMVC3000,RawCallBack,this);
    MV_Usb2SetFrameCallBack(m_hMVC3000,FrameCallBack,this);

    MV_Usb2SetImageProcess(m_hMVC3000,m_nBrightness,m_nContrast,0,m_nSaturation,TRUE);
    ui->statusBar->showMessage("USB相机初始化成功");
}

void MVCCamera::onShowAboutDlg()
{
    AboutDlg aboutDlg;
    aboutDlg.exec();
}

void MVCCamera::onStartCapActionTriggered()
{
    if(m_hMVC3000 == NULL)
    {
        QMessageBox msgBox(QMessageBox::Warning,"提示",\
                           "首先请初始化MVC相机",QMessageBox::Yes,this);
        msgBox.exec();
        return;
    }
    wchar_t *wstr;
    QString("MVC相机预览").toWCharArray(wstr);
    MV_Usb2Start(m_hMVC3000,wstr,\
                 WS_OVERLAPPEDWINDOW|WS_VISIBLE,\
                 100,100,-1,-1,0,0,\
                 THREAD_PRIORITY_NORMAL,\
                 THREAD_PRIORITY_NORMAL);

    m_bPreview = TRUE;
    m_bPause = FALSE;

    MV_Usb2SetPartOfCapInfo(m_hMVC3000,&m_CapInfo);
    MV_Usb2SetBw(m_hMVC3000,m_bBw);
}

void MVCCamera::onPauseCapActionTriggered()
{
    if(!m_bConnect)
    {
        QMessageBox msgBox;
        msgBox.setText("请先连接相机");
        msgBox.exec();
        return;
    }

    int rt = MV_Usb2PausePreview(m_hMVC3000,m_bPause);
    Q_UNUSED(rt);
    m_bPause = !m_bPause;

    // 改变菜单上的提示
    if(m_bPause)
    {
        pauseCapAction->setText(tr("运行"));
        pauseCapAction->setStatusTip("重新启动预览");
    }
    else
    {
        pauseCapAction->setText(tr("暂停"));
        pauseCapAction->setStatusTip("暂停预览");
    }
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
