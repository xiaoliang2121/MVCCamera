#ifndef MVCCAMERA_H
#define MVCCAMERA_H

#include <QMainWindow>
#include <windows.h>
//#include <winbase.h>
//#include <winuser.h>
#include <MVCAPI.h>
#include <QActionGroup>

namespace Ui {
class MVCCamera;
}

class MVCCamera : public QMainWindow
{
    Q_OBJECT

public:
    explicit MVCCamera(QWidget *parent = 0);
    ~MVCCamera();

    void setNewMenu();
    void createActions();
    void createMenus();

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

public slots:
    void onConnectActionTriggered();
    void onShowAboutDlg();

    void onStartCapActionTriggered();
    void onPauseCapActionTriggered();
    void onStopCapActionTriggered();

    void onContinueModeTriggered();
    void onTrigModeTriggered();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MVCCamera *ui;

    QAction *connectAction;
    QAction *quitAction;
    QAction *aboutAction;

    QAction *exitAction;
    QAction *startCapAction;
    QAction *pauseCapAction;
    QAction *stopCapAction;

    QAction *continueMode;
    QAction *trigMode;
    QActionGroup *group;

};

#endif // MVCCAMERA_H
