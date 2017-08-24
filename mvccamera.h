#ifndef MVCCAMERA_H
#define MVCCAMERA_H

#include <QMainWindow>
#include <MVCAPI.h>

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

    HANDLE m_hMVC3000;
    CapInfoStruct m_CapInfo;        // 视频属性

public slots:
    void onConnectActionTriggered();
    void onShowAboutDlg();

    void onStartCapActionTriggered();
    void onPauseCapActionTriggered();
    void onStopCapActionTriggered();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MVCCamera *ui;

    QAction *connectAction;
    QAction *aboutAction;

    QAction *exitAction;
    QAction *startCapAction;
    QAction *pauseCapAction;
    QAction *stopCapAction;

};

#endif // MVCCAMERA_H
