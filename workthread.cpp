#include "workthread.h"

WorkThread::WorkThread()
{

}

WorkThread::~WorkThread()
{
    requestInterruption();
    quit();
    wait();
}
