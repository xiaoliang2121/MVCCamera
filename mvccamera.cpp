#include "mvccamera.h"
#include "ui_mvccamera.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QToolBar>
#include "aboutdlg.h"
#include "trigsettingsdlg.h"
#include "gammavalset.h"

static LONG gGains;
static LONG gExposure;
MVCCamera *gMVC;

MVCCamera::MVCCamera(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MVCCamera)
{
    ui->setupUi(this);
    QVBoxLayout *layout = new QVBoxLayout;
    ui->centralWidget->setLayout(layout);
    Camera_label = new QLabel(this);
    Camera_label->setStyleSheet("background-color: rgb(107, 107, 107)");
    layout->addWidget(Camera_label);

    gMVC = this;

    // 线程初始化
    m_AutoEx.moveToThread(&m_ExOnWork);
    connect(this,&MVCCamera::AeTriggered,\
            &m_AutoEx,&AutoExposure::DoAutoEx);

    m_AutoWB.moveToThread(&m_WBOnWork);
    connect(this,&MVCCamera::AwbTriggered,\
            &m_AutoWB,&AutoWhiteBalance::DoAutoWB);

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
    m_bRawSave =  FALSE;
    m_bRawToRGB = FALSE;
    m_bRGBSave = FALSE;

    // 内存分配
    DWORD RGBDataSize = MAXWIDTH*MAXHEIGHT*3;
    m_pRGBData = (BYTE*)malloc(RGBDataSize*sizeof(BYTE));
    memset(m_pRGBData,0,RGBDataSize);
    DWORD RawDataSize = MAXWIDTH*MAXHEIGHT;
    m_pRawData=(BYTE*)malloc(RawDataSize*sizeof(BYTE));
    memset(m_pRawData,0,RawDataSize);
//    m_CapInfo.Buffer = m_pRawData;
    InitImageParam();

    // 顶层菜单设置
    setTopMenu();
}

MVCCamera::~MVCCamera()
{
    delete ui;
}

void MVCCamera::setTopMenu()
{
    connectAction = new QAction(tr("连接"),this);
    connectAction->setStatusTip("通过USB2.0连接到相机");
    connect(connectAction,&QAction::triggered,this,\
            &MVCCamera::onConnectActionTriggered);

    quitAction = new QAction(tr("退出"),this);
    quitAction->setStatusTip("退出程序");
    connect(quitAction,&QAction::triggered,\
            this,&MVCCamera::close);

    aboutAction = new QAction(tr("关于MVC_CAMERA程序"));
    aboutAction->setStatusTip("程序相关");
    connect(aboutAction,&QAction::triggered,\
            this,&MVCCamera::onShowAboutDlg);

    QMenu *connectMenu = menuBar()->addMenu(tr("USB2.0连接"));
    connectMenu->addAction(connectAction);
    connectMenu->addAction(quitAction);
    QMenu *helpMenu = menuBar()->addMenu("帮助");
    helpMenu->addAction(aboutAction);
}

void MVCCamera::setNewMenu()
{
    menuBar()->clear();

    // 创建新的Action
    createActions();

    // 将Action添加进菜单
    createMenus();

    // 将Action添加进工具栏
    createTools();
}

void MVCCamera::createActions()
{
    // 相机开始、暂停、停止操作
    startCapAction = new QAction(QIcon(":/icon/start.ico"),tr("启动"),this);
    startCapAction->setStatusTip("启动相机，开始采集或者预览");
    connect(startCapAction,&QAction::triggered,\
            this,&MVCCamera::onStartCapActionTriggered);

    pauseCapAction = new QAction(QIcon(":/icon/pause.ico"),tr("暂停"),this);
    pauseCapAction->setStatusTip("暂停预览");
    connect(pauseCapAction,&QAction::triggered,\
            this,&MVCCamera::onPauseCapActionTriggered);

    stopCapAction = new QAction(QIcon(":/icon/stop.ico"),tr("停止"),this);
    stopCapAction->setStatusTip("停止相机，以及一些收尾工作");
    connect(stopCapAction,&QAction::triggered,\
            this,&MVCCamera::onStopCapActionTriggered);

    exitAction = new QAction(tr("退出程序"),this);\
    exitAction->setStatusTip("退出本程序");
    connect(exitAction,&QAction::triggered,\
            this,&MVCCamera::close);

    // 相机模式切换
    continueMode = new QAction(tr("连续模式"),this);
    continueMode->setCheckable(true);
    continueMode->setStatusTip("切换到连续模式");
    connect(continueMode,&QAction::triggered,this,\
            &MVCCamera::onContinueModeTriggered);

    trigMode = new QAction(tr("触发模式"),this);
    trigMode->setCheckable(true);
    trigMode->setStatusTip("切换到触发模式");
    connect(trigMode,&QAction::triggered,\
            this,&MVCCamera::onTrigModeTriggered);

    trigModeSettings = new QAction(QIcon(":/icon/param.ICO"),tr("触发设置"),this);
    trigModeSettings->setStatusTip("触发模式下的参数设置");
    connect(trigModeSettings,&QAction::triggered,\
            this,&MVCCamera::onTrigModeSettingsTriggered);

    group = new QActionGroup(this);
    group->addAction(continueMode);
    group->addAction(trigMode);
    continueMode->setChecked(true);

    // 视频调整和预览设置
    autoExposuse = new QAction(tr("自动曝光"),this);
    autoExposuse->setStatusTip("对相机进行自动曝光操作");
    connect(autoExposuse,&QAction::triggered,\
            this,&MVCCamera::onAutoExposureTriggered);

    autoWhiteBalance = new QAction(tr("自动白平衡"),this);
    autoWhiteBalance->setStatusTip("对相机进行自动曝光操作");
    connect(autoWhiteBalance,&QAction::triggered,\
            this,&MVCCamera::onAutoWhiteBalanceTriggered);

    GammaCorrection = new QAction(QIcon(":/icon/colorcorrect.png"),tr("伽马值正"),this);
    GammaCorrection->setStatusTip("进行Gamma值校正");
    connect(GammaCorrection,&QAction::triggered,\
            this,&MVCCamera::onGammaCorrectionTriggered);

    bwAction = new QAction(QIcon(":/icon/bwShow.png"),tr("黑白显示"),this);
    bwAction->setStatusTip("黑白/彩色显示切换");
    connect(bwAction,&QAction::triggered,\
            this,&MVCCamera::onBwActionTriggered);
}

void MVCCamera::createMenus()
{
    QMenu *operation = menuBar()->addMenu(tr("操作"));
    operation->addAction(startCapAction);
    operation->addAction(pauseCapAction);
    operation->addAction(stopCapAction);
    operation->addSeparator();
    operation->addAction(exitAction);

    QMenu *ModeSelection = menuBar()->addMenu(tr("模式选择"));
    ModeSelection->addAction(continueMode);
    ModeSelection->addAction(trigMode);
    ModeSelection->addSeparator();
    ModeSelection->addAction(trigModeSettings);

    QMenu *VideoOperation = menuBar()->addMenu(tr("视频"));
    VideoOperation->addAction(autoExposuse);
    VideoOperation->addAction(autoWhiteBalance);
    VideoOperation->addSeparator();
    VideoOperation->addAction(bwAction);
    VideoOperation->addSeparator();
    VideoOperation->addAction(GammaCorrection);
}

void MVCCamera::createTools()
{
    QToolBar *operationTool = addToolBar(tr("操作"));
    operationTool->addAction(startCapAction);
    operationTool->addAction(pauseCapAction);
    operationTool->addAction(stopCapAction);

    QToolBar *VideoTool = addToolBar(tr("视频"));
    VideoTool->addAction(bwAction);
    VideoTool->addAction(GammaCorrection);
}

void MVCCamera::InitImageParam()
{
    memset(&m_CapInfo, 0, sizeof(CapInfoStruct));
    m_CapInfo.Buffer = m_pRawData;

    m_CapInfo.Width		= 800;
    m_CapInfo.Height	= 600;
    m_CapInfo.HorizontalOffset = 0;
    m_CapInfo.VerticalOffset   = 0;
    m_CapInfo.Exposure         = 100;
    m_CapInfo.Gain[0]          = 17;
    m_CapInfo.Gain[1]          = 9;
    m_CapInfo.Gain[2]          = 15;
    m_CapInfo.Control          = 0;
    memset(m_CapInfo.Reserved, 0, 8);
    m_CapInfo.Reserved[0]	   = 2;

    m_rectPreview.setTop(0);
    m_rectPreview.setLeft(0);
    m_rectPreview.setWidth(m_CapInfo.Width);
    m_rectPreview.setHeight(m_CapInfo.Height);

    m_AutoEx.m_CapInfo = m_CapInfo;
    m_AutoWB.m_CapInfo = m_CapInfo;
}

int MVCCamera::FrameCallBackFunc(BYTE *pBGR)
{
    // 保存为Bmp文件
    if (m_bRGBSave) {
        saveRGBAsBmp(pBGR, "RGBData.bmp", m_CapInfo.Width,m_CapInfo.Height);
        m_bRGBSave=FALSE;
    }

    return 0;
}

int MVCCamera::saveRGBAsBmp(BYTE *pSrc, QString FileName, DWORD dwWidth, DWORD dwHeight)
{
    FILE *fp;
    BITMAPFILEHEADER pf;
    BITMAPINFOHEADER FrameBmi;

    if(pSrc == NULL)
        return 0;

    DWORD Width = dwWidth;
    DWORD Height = dwHeight;
    DWORD RowLength;
    RowLength=4*((Width*24+31)/32);

    DWORD RGBFrameSize= RowLength*Height;

    FrameBmi.biSize=sizeof(BITMAPINFOHEADER);
    FrameBmi.biPlanes=1;
    FrameBmi.biCompression=BI_RGB;
    FrameBmi.biClrImportant=0;
    FrameBmi.biSizeImage=0;
    FrameBmi.biClrUsed=0;//use biBitCount value
    FrameBmi.biBitCount=24;
    FrameBmi.biWidth=Width;
    FrameBmi.biHeight=Height;
    FrameBmi.biXPelsPerMeter = 0;
    FrameBmi.biYPelsPerMeter =0;

    QByteArray FileName_ = FileName.toLocal8Bit();
    fp = fopen(FileName_.data(), "wb");
    if (fp == NULL){
        QMessageBox msgBox;
        msgBox.setText("ERROR: 创建文件出现错误！");
        msgBox.exec();
        return -1;
    }

    pf.bfType=0x4d42; //"BM"
    pf.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+RGBFrameSize;
    pf.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
    pf.bfReserved1=0;
    pf.bfReserved2=0;

    RGBQUAD *pPallete = (RGBQUAD*)malloc(256*sizeof(RGBQUAD));
    for(int i=0 ; i<256; i++){
        pPallete[i].rgbRed = (BYTE)i;
        pPallete[i].rgbBlue = (BYTE)i;
        pPallete[i].rgbGreen = (BYTE)i;
        pPallete[i].rgbReserved = (BYTE)0;
    }
    fwrite(&pf,sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
    fwrite(&FrameBmi,sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
    fwrite(pSrc,sizeof(BYTE),RGBFrameSize,fp);
    fclose(fp);
    free(pPallete);

    return 0;
}

void CALLBACK AWBFunction(LPVOID pParam)
{
    Q_UNUSED(pParam);

    // 这里使用线程来处理
    if(!gMVC->m_WBOnWork.isRunning())
        gMVC->m_WBOnWork.start();

    emit gMVC->AwbTriggered(gGains);

    // 更新m_CapInfo中参数
    gMVC->m_CapInfo.Gain[0] = gGains & 0xff;
    gMVC->m_CapInfo.Gain[1] = (gGains >> 8) & 0xff;
    gMVC->m_CapInfo.Gain[2] = (gGains >> 16) & 0xff;
}

void CALLBACK AEFunction(LPVOID pParam)
{
    Q_UNUSED(pParam);

    // 这里使用线程来处理
    if(!gMVC->m_ExOnWork.isRunning())
        gMVC->m_ExOnWork.start();

    emit gMVC->AeTriggered(gExposure);

    // 更新m_CapInfo中参数
    gMVC->m_CapInfo.Exposure = gExposure;
}

void CALLBACK RawCallBack(LPVOID lpParam, LPVOID lpUser)
{
    BYTE *pDataBuffer = (BYTE *)lpParam;
    MVCCamera *pMVCCamera = (MVCCamera *)lpUser;

    // 保存Raw数据
    if(pMVCCamera->m_bRawSave)
    {
        // 保存操作
        FILE * fp;
        fp = fopen("RawData.raw", "wb+");
        if(fp)
            fwrite(pDataBuffer,sizeof(BYTE),\
                   pMVCCamera->m_CapInfo.Width*pMVCCamera->m_CapInfo.Height,\
                   fp);
        fclose(fp);
        pMVCCamera->m_bRawSave = FALSE;
    }

    // 转换为RGB数据，以便后续操作
    if (pMVCCamera->m_bRawToRGB)
    {
        MV_Usb2ConvertRawToRgb(pMVCCamera->m_hMVC3000,pDataBuffer,\
                               pMVCCamera->m_CapInfo.Width,\
                               pMVCCamera->m_CapInfo.Height,\
                               pMVCCamera->m_pRGBData);
        pMVCCamera->saveRGBAsBmp(pMVCCamera->m_pRGBData, "RawToRGBData.bmp",\
                               pMVCCamera->m_CapInfo.Width,\
                               pMVCCamera->m_CapInfo.Height);
        pMVCCamera->m_bRawToRGB = FALSE;
    }
}

void CALLBACK FrameCallBack(LPVOID lpParam, LPVOID lpUser)
{
    BYTE *pDataBuffer = (BYTE*)lpParam;
    MVCCamera *pMVCCamera = (MVCCamera *)lpUser;

    pMVCCamera->FrameCallBackFunc(pDataBuffer);
}

void MVCCamera::onConnectActionTriggered()
{
    setNewMenu();           // 测试时使用，完成后删除
    // 进行相机连接的初始化操作
    if(m_bConnect)
        return;

    int nIndex = 0;
    int rt = MV_Usb2Init("MVC-F",&nIndex,&m_CapInfo,&m_hMVC3000);
    if(ResSuccess != rt){
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("提示");
        msgBox.setText("错误：无法初始化USB相机！");
        msgBox.exec();
        MV_Usb2Uninit(&m_hMVC3000);
        m_hMVC3000 = NULL;
        return;
    }

    // 设置新菜单
//    setNewMenu();

    // 显示设备数(先处理只有一个相机的情况)
    MV_Usb2GetNumberDevices(m_hMVC3000,&m_nDeviceNum);
//    LPTSTR temp;
//    MV_Usb2GetSerial(m_hMVC3000,temp);
//    QString str;
//    str.fromLatin1(temp);
//    m_strDeviceNum = QString("MVC3000F Num:%1, S/N:%2").arg(m_nDeviceNum).arg(str);

    m_bConnect = TRUE;
    m_AutoEx.m_hMVC3000 = m_hMVC3000;
    m_AutoWB.m_hMVC3000 = m_hMVC3000;

    emit onContinueModeTriggered();

    // 设置回调函数
    MV_Usb2SetAwbCallBackFunction(m_hMVC3000,180,180,180,\
                                  reinterpret_cast<LPVOID>(AWBFunction),&gGains);  // 自动白平衡
    MV_Usb2SetAeCallBackFunction(m_hMVC3000,180,\
                                 reinterpret_cast<LPVOID>(AEFunction),&gExposure);         // 自动曝光操作

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

    HWND hwnd = (HWND)Camera_label->winId();

    MV_Usb2Start(m_hMVC3000,reinterpret_cast<LPCTSTR>(QString("MVC相机预览").utf16()),\
                 WS_CHILD|WS_VISIBLE,\
                 m_rectPreview.left(),\
                 m_rectPreview.top(),\
                 m_rectPreview.width(),\
                 m_rectPreview.height(),\
                 hwnd,\
                 0,\
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
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("请先连接相机");
        msgBox.setWindowTitle("提示");
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
    if(m_bPreview)
    {
        int rt = MV_Usb2Stop(m_hMVC3000);
        if(ResSuccess == rt)
        {
            m_bPreview	= FALSE;
            m_bPause	= FALSE;
        }
    }
}

void MVCCamera::onContinueModeTriggered()
{
    if(!m_bConnect)
        return;

    if(m_hMVC3000 == NULL)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("请初始化相机");
        msgBox.setWindowTitle("提示");
        msgBox.exec();
    }

    m_nOpMode = 0;
    int rt = MV_Usb2SetOpMode(m_hMVC3000,m_nOpMode,FALSE);
    if(rt != 0)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("设置相机模式失败");
        msgBox.setWindowTitle("提示");
        msgBox.exec();
    }
}

void MVCCamera::onTrigModeTriggered()
{
    if(!m_bConnect)
        return;

    if(m_hMVC3000 == NULL)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("请初始化相机");
        msgBox.setWindowTitle("提示");
        msgBox.exec();
    }

    m_nOpMode = 1;
    int rt = MV_Usb2SetOpMode(m_hMVC3000,m_nOpMode,FALSE);
    if(rt != 0)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("设置相机模式失败");
        msgBox.setWindowTitle("提示");
        msgBox.exec();
    }
}

void MVCCamera::onTrigModeSettingsTriggered()
{
    TrigSettingsDlg dlg;
    dlg.m_hMVC3000 = m_hMVC3000;
    dlg.m_bConnect = m_bConnect;
    dlg.exec();
}

void MVCCamera::onAutoExposureTriggered()
{
    int rt = MV_Usb2AutoExposure(m_hMVC3000,TRUE,180,\
                        reinterpret_cast<LPVOID>(AEFunction),&gExposure);
    if(ResSuccess != rt)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("自动曝光执行失败");
        msgBox.setWindowTitle("提示");
        msgBox.exec();
    }
}

void MVCCamera::onAutoWhiteBalanceTriggered()
{
    int rt = MV_Usb2AWB(m_hMVC3000,TRUE,180,180,180,\
               reinterpret_cast<LPVOID>(AWBFunction),&gGains);
    if(ResSuccess != rt)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("自动白平衡执行失败");
        msgBox.setWindowTitle("提示");
        msgBox.exec();
    }
}

void MVCCamera::onGammaCorrectionTriggered()
{
    GammaValSet dlg;
    dlg.m_hMVC3000 = m_hMVC3000;
    dlg.fValue = 1.0;
    dlg.exec();
}

void MVCCamera::onBwActionTriggered()
{
    if(!m_bConnect)
        return;

    if(!m_bBw)
    {
        m_bBw = TRUE;
        bwAction->setText(tr("彩色显示"));
        bwAction->setStatusTip("切换为彩色显示");

    }
    else
    {
        m_bBw = FALSE;
        bwAction->setText(tr("黑白显示"));
        bwAction->setStatusTip("切换为黑白显示");
    }

    MV_Usb2SetBw(m_hMVC3000,m_bBw);
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
