#include "mvccamera.h"
#include "ui_mvccamera.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QToolBar>
#include "aboutdlg.h"
#include "trigsettingsdlg.h"
#include "gammavalset.h"
#include <QDebug>

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

    // 供回调函数使用
    gMVC = this;
    m_imgCount = 0;

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
    m_CapImgNum = CAPIMGNUM;       // 暂定要采集图像50张，开辟50块图像内存，指向内存的指针存在Vector中
    m_nBrightness= 0;
    m_nContrast	 = 20;
    m_nSaturation= 100;
    m_bPreview = FALSE;
    m_bPause   = FALSE;
    m_bBw = FALSE;
    m_bRawSave =  FALSE;
//    m_bRawToRGB = FALSE;
    m_bRGBSave = FALSE;

    // 内存分配
    DWORD RGBDataSize = MAXWIDTH*MAXHEIGHT*3;
    m_pRGBData = (BYTE*)malloc(RGBDataSize*sizeof(BYTE));
    memset(m_pRGBData,0,RGBDataSize);
    DWORD RawDataSize = MAXWIDTH*MAXHEIGHT;
//    m_pRawData=(BYTE*)malloc(RawDataSize*sizeof(BYTE));
//    memset(m_pRawData,0,RawDataSize);
//    m_CapInfo.Buffer = m_pRawData;

    // 各开辟50块图像内存，指向内存的指针存在Vector中

    for(uint i=0;i<m_CapImgNum;++i)
    {
        uchar *pTemp = (uchar*)malloc(RawDataSize*sizeof(uchar));
        memset(pTemp,0,RawDataSize);
        m_pRawDataVec.push_back(pTemp);
    }

    InitImageParam();

    // 顶层菜单设置
    setTopMenu();
}

MVCCamera::~MVCCamera()
{
    delete ui;

    // 释放malloc开辟的内存
    free(m_pRGBData);
    //free(m_pRawData);

    for(int i=0;i<m_pRawDataVec.size();++i)
        free(m_pRawDataVec[i]);
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

    softTrigger = new QAction(QIcon(":/icon/start_GetImg.png"),tr("开始采集"),this);
    softTrigger->setStatusTip("在触发模式下，通过软件发送触发信号给摄像头，"
                              "采集一帧图像（这里用于启动整个采集工作）");
    connect(softTrigger,&QAction::triggered,\
            this,&MVCCamera::onSoftTrigTriggered);

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

    GammaCorrection = new QAction(QIcon(":/icon/colorcorrect.png"),tr("伽马校正"),this);
    GammaCorrection->setStatusTip("进行Gamma值校正");
    connect(GammaCorrection,&QAction::triggered,\
            this,&MVCCamera::onGammaCorrectionTriggered);

    bwAction = new QAction(QIcon(":/icon/bwShow.png"),tr("黑白显示"),this);
    bwAction->setStatusTip("黑白/彩色显示切换");
    connect(bwAction,&QAction::triggered,\
            this,&MVCCamera::onBwActionTriggered);

    // 采集图像
    capFrame = new QAction(QIcon(":/icon/buhuo.png"),tr("捕获连续帧"),this);
    capFrame->setStatusTip("在触发模式下，进行连续帧的捕获");
    connect(capFrame,&QAction::triggered,\
            this,&MVCCamera::onCapFrameTriggered);

    stopCapImg = new QAction(QIcon(":/icon/stopimg.png"),tr("停止采集"),this);
    stopCapImg->setEnabled(false);
    stopCapImg->setStatusTip("触发模式下，停止采集");
    connect(stopCapImg,&QAction::triggered,\
            this,&MVCCamera::onStopCapImgTriggered);
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
    ModeSelection->addAction(softTrigger);
    ModeSelection->addAction(trigModeSettings);

    QMenu *VideoOperation = menuBar()->addMenu(tr("视频"));
    VideoOperation->addAction(autoExposuse);
    VideoOperation->addAction(autoWhiteBalance);
    VideoOperation->addSeparator();
    VideoOperation->addAction(bwAction);
    VideoOperation->addSeparator();
    VideoOperation->addAction(GammaCorrection);

    QMenu *CapOperation = menuBar()->addMenu(tr("采集图像"));
    CapOperation->addAction(capFrame);
    CapOperation->addAction(stopCapImg);
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
    VideoTool->addSeparator();
    VideoTool->addAction(capFrame);
    VideoTool->addAction(softTrigger);
    VideoTool->addAction(stopCapImg);
}

void MVCCamera::InitImageParam()
{
    memset(&m_CapInfo, 0, sizeof(CapInfoStruct));
    m_CapInfo.Buffer = m_pRawDataVec[0];

    m_CapInfo.Width		= 800;
    m_CapInfo.Height	= 600;
    m_CapInfo.HorizontalOffset = 0;
    m_CapInfo.VerticalOffset   = 0;
    m_CapInfo.Exposure         = 150;           // 设置触发模式下，曝光时间为15ms
    m_CapInfo.Gain[0]          = 13;
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

    qDebug()<<"增益"<<"("<<gMVC->m_CapInfo.Gain[0]<<","\
           <<gMVC->m_CapInfo.Gain[1]<<","\
          <<gMVC->m_CapInfo.Gain[2]<<")"<<endl;
}

void CALLBACK AEFunction(LPVOID pParam)
{
    Q_UNUSED(pParam);

    // 这里使用线程来处理
    if(!gMVC->m_ExOnWork.isRunning())
        gMVC->m_ExOnWork.start();

    emit gMVC->AeTriggered(gExposure);

    // 更新m_CapInfo中参数
//    gMVC->m_CapInfo.Exposure = gExposure;
    qDebug()<<"曝光"<<gExposure/*gMVC->m_CapInfo.Exposure*/<<endl;
}

void CALLBACK RawCallBack(LPVOID lpParam, LPVOID lpUser)
{
    BYTE *pDataBuffer = (BYTE *)lpParam;
    MVCCamera *pMVCCamera = (MVCCamera *)lpUser;
    Q_UNUSED(pDataBuffer);      // 这里图像已经在开辟的内存中了，留待使用

    if(pMVCCamera->m_nOpMode == 0)
        return;

    // 保存Raw数据
    if(pMVCCamera->m_bRawSave)
    {
        pMVCCamera->m_imgCount++;
        pMVCCamera->m_CapInfo.Buffer = pMVCCamera->m_pRawDataVec.at\
                (pMVCCamera->m_imgCount);
        MV_Usb2SetPartOfCapInfo(pMVCCamera->m_hMVC3000,
                                &(pMVCCamera->m_CapInfo));      // 实际改变图片缓存位置

        QString fileName = QString("gray%1").arg(pMVCCamera->m_imgCount);
        fileName = fileName + ".bmp";

        QImage img = QImage(pDataBuffer,pMVCCamera->m_CapInfo.Width,
                            pMVCCamera->m_CapInfo.Height,
                            QImage::Format_Grayscale8);
        img.save(fileName);
        qDebug()<<"保存图片"<<fileName<<endl;
    }
}

void CALLBACK FrameCallBack(LPVOID lpParam, LPVOID lpUser)
{
    BYTE *pDataBuffer = (BYTE*)lpParam;
    MVCCamera *pMVCCamera = (MVCCamera *)lpUser;

    if(pMVCCamera->m_nOpMode == 0)
        return;

    // 保存为Bmp文件
    if (pMVCCamera->m_bRGBSave)
    {
        pMVCCamera->m_imgCount++;
        QString fileName = QString("RGB%1").arg(pMVCCamera->m_imgCount);
        fileName = fileName + ".bmp";

        QImage img = QImage(pDataBuffer,pMVCCamera->m_CapInfo.Width,\
                            pMVCCamera->m_CapInfo.Height,\
                            QImage::Format_RGB888);
        img.save(fileName);
        qDebug()<<"保存图片"<<fileName<<endl;
    }
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

    // 默认状态
    emit onContinueModeTriggered();

    // 设置回调函数
    MV_Usb2SetAwbCallBackFunction(m_hMVC3000,180,180,180,\
                                  reinterpret_cast<LPVOID>(AWBFunction),&gGains);   // 自动白平衡
    MV_Usb2SetAeCallBackFunction(m_hMVC3000,180,\
                                 reinterpret_cast<LPVOID>(AEFunction),&gExposure);  // 自动曝光操作

    MV_Usb2SetRawCallBack(m_hMVC3000,RawCallBack,this);
    MV_Usb2SetFrameCallBack(m_hMVC3000,FrameCallBack,this);

    // 其它操作
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
    int rt = MV_Usb2SetOpMode(m_hMVC3000,m_nOpMode,FALSE); // 连续模式时，不启用闪光信号
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
    int rt = MV_Usb2SetOpMode(m_hMVC3000,m_nOpMode,TRUE);   // 触发模式，启用闪光信号
    if(rt != 0)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("设置相机模式失败");
        msgBox.setWindowTitle("提示");
        msgBox.exec();
    }

    // 触发模式下的参数更新
    MV_Usb2SetPartOfCapInfo(m_hMVC3000,&m_CapInfo);
    emit onTrigModeSettingsTriggered();
}

void MVCCamera::onSoftTrigTriggered()
{
    if(!m_bConnect)
        return;

    int rt = MV_Usb2SendUserTrigger(m_hMVC3000);
    if(rt != 0)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("软触发执行失败");
        msgBox.setWindowTitle("提示");
        msgBox.exec();
    }
}

void MVCCamera::onTrigModeSettingsTriggered()
{
    TrigSettingsDlg dlg;
    dlg.m_hMVC3000 = m_hMVC3000;
    dlg.m_bConnect = m_bConnect;
    dlg.m_CapInfo = m_CapInfo;

    connect(&dlg,&TrigSettingsDlg::updateExposure,\
            this,&MVCCamera::on_Update_Exp);                        // 更新Exposure参数

    dlg.exec();
}

void MVCCamera::onAutoExposureTriggered()
{
    // 只在连续模式下使用,调节相机参数
    if(m_nOpMode != 0)
        return;

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
    // 只在连续模式下使用,调节相机参数
    if(m_nOpMode != 0)
        return;

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

void MVCCamera::onCapFrameTriggered()
{
    if(m_nOpMode)
    {
        // 将采集帧数清为零
        m_imgCount = 0;

        if(m_bBw)
        {
            // 为真时，黑白图像
            m_bRawSave = TRUE;
            m_bRGBSave = FALSE;
        }
        else
        {
            // 为假时，彩色图像
            m_bRawSave = FALSE;
            m_bRGBSave = TRUE;
        }

        capFrame->setEnabled(false);
        stopCapImg->setEnabled(true);
    }
}

void MVCCamera::onStopCapImgTriggered()
{
    m_bRawSave = FALSE;
    m_bRGBSave = FALSE;

    m_imgCount = 0; // 清零

    capFrame->setEnabled(true);
    stopCapImg->setEnabled(false);
}

void MVCCamera::on_Update_Exp(ulong exp)
{
    m_CapInfo.Exposure = exp;
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
