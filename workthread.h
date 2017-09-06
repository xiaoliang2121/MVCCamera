#ifndef WORKTHREAD_H
#define WORKTHREAD_H

#include <QThread>

class WorkThread : public QThread
{
public:
    WorkThread();
    ~WorkThread();
};

#endif // WORKTHREAD_H
