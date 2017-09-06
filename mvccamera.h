﻿#ifndef MVCCAMERA_H
#define MVCCAMERA_H

#include <QMainWindow>
#include <windows.h>
//#include <winbase.h>
//#include <winuser.h>
#include <MVCAPI.h>
#include <QActionGroup>
#include <QLabel>
#include "workthread.h"
#include "autoexposure.h"
#include "autowhitebalance.h"

namespace Ui {
class MVCCamera;
}

class MVCCamera : public QMainWindow
{
    Q_OBJECT

public:
    explicit MVCCamera(QWidget *parent = 0);
    ~MVCCamera();

    void setTopMenu();
    void setNewMenu();
    void createActions();
    void createMenus();
    void createTools();
    void InitImageParam();
    int FrameCallBackFunc(BYTE *pBGR);
    int saveRGBAsBmp(BYTE * pSrc, QString FileName,\
                     DWORD dwWidth, DWORD dwHeight);

    HANDLE m_hMVC3000;
    CapInfoStruct m_CapInfo;        // 视频属性
    int m_nOpMode;                  // 模式[连续或者触发]
    BOOL m_bConnect;
    BYTE* m_pRGBData;               // 24bitRGB数据指针
    BYTE* m_pRawData;               // 用于存放RawData数据
    ULONG  m_nDeviceNum;            // 当前连接的相机数量
    QString m_strDeviceNum;
    DWORD MAXWIDTH;
    DWORD MAXHEIGHT;
    int m_nBrightness, m_nContrast,m_nSaturation;       // 亮度、对比度、饱和度
    BOOL m_bPreview;
    BOOL m_bPause;
    BOOL m_bBw;
    // 标志位
    BOOL m_bRawSave;
    BOOL m_bRawToRGB;
    BOOL m_bRGBSave;

public:
    void CALLBACK AWBFunction(LPVOID pParam);
    void CALLBACK AEFunction(LPVOID pParam);

public slots:
    void onConnectActionTriggered();
    void onShowAboutDlg();

    void onStartCapActionTriggered();
    void onPauseCapActionTriggered();
    void onStopCapActionTriggered();

    void onContinueModeTriggered();
    void onTrigModeTriggered();
    void onTrigModeSettingsTriggered();

    void onAutoExposureTriggered();
    void onAutoWhiteBalanceTriggered();
    void onBwActionTriggered();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void AwbTriggered(LONG value);
    void AeTriggered(LONG value);

private:
    Ui::MVCCamera *ui;
    QLabel *Camera_label;
    QRect m_rectPreview;

    // 自动曝光和自动白平衡线程
    WorkThread m_ExOnWork;
    WorkThread m_WBOnWork;
    AutoExposure m_AutoEx;
    AutoWhiteBalance m_AutoWB;

    QAction *connectAction;
    QAction *quitAction;
    QAction *aboutAction;

    QAction *exitAction;
    QAction *startCapAction;
    QAction *pauseCapAction;
    QAction *stopCapAction;

    QAction *continueMode;
    QAction *trigMode;
    QAction *trigModeSettings;
    QActionGroup *group;

    QAction *autoExposuse;
    QAction *autoWhiteBalance;
    QAction *bwAction;
};

#endif // MVCCAMERA_H
