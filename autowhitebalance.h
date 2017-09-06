#ifndef AUTOWHITEBALANCE_H
#define AUTOWHITEBALANCE_H

#include <QObject>
#include <windows.h>
#include "MVCAPI.h"

class AutoWhiteBalance : public QObject
{
    Q_OBJECT
public:
    explicit AutoWhiteBalance(QObject *parent = 0);

    HANDLE m_hMVC3000;
    CapInfoStruct m_CapInfo;

signals:

public slots:
    void DoAutoWB(long gains);
};

#endif // AUTOWHITEBALANCE_H
