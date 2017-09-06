#include "autowhitebalance.h"

AutoWhiteBalance::AutoWhiteBalance(QObject *parent) : QObject(parent)
{

}

void AutoWhiteBalance::DoAutoWB(long gains)
{
    m_CapInfo.Gain[0] = gains & 0xff;
    m_CapInfo.Gain[1] = (gains >> 8) & 0xff;
    m_CapInfo.Gain[2] = (gains >> 16) & 0xff;
    MV_Usb2SetCapInfo(m_hMVC3000,&m_CapInfo);
}
