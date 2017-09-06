#include "autoexposure.h"

AutoExposure::AutoExposure(QObject *parent) : QObject(parent)
{

}

void AutoExposure::DoAutoEx(long exposure)
{
    m_CapInfo.Exposure = exposure;
    MV_Usb2SetCapInfo(m_hMVC3000,&m_CapInfo);
}
