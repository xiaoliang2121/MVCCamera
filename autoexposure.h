#ifndef AUTOEXPOSURE_H
#define AUTOEXPOSURE_H

#include <QObject>
#include <windows.h>
#include "MVCAPI.h"

class AutoExposure : public QObject
{
    Q_OBJECT
public:
    explicit AutoExposure(QObject *parent = 0);

    HANDLE m_hMVC3000;
    CapInfoStruct m_CapInfo;

signals:

public slots:
    void DoAutoEx(long exposure);
};

#endif // AUTOEXPOSURE_H
