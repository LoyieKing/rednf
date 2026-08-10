#ifndef DBMW_DNFTHREADINTERFACE_H_
#define DBMW_DNFTHREADINTERFACE_H_

#include "DBMWCommon.h"


class CThreadInterface
{
public:
    CThreadInterface();
    virtual void stop();
    virtual void join();
    virtual ~CThreadInterface();
    virtual void dispatch(void* param) = 0;
    bool begin();
    void* dispatch_proxy(void* param);
    pthread_t m_thread;  // +4
    bool m_stop;         // +8（ORIG bool：循环检查为 test+jne，char 会多 setne）
};

#endif  // DBMW_DNFTHREADINTERFACE_H_
