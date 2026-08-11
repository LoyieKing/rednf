#ifndef DNF_APP_THREAD_H_
#define DNF_APP_THREAD_H_

#include <queue>

#include "DNFTickHandler.h"
#include "DNFThreadInterface.h"

class CApplication;
class CMutex;
class CUdpRecvBuffer;

// CAppThread：CThreadInterface@0 + app@0xc + CFrameCountHandler@0x10 +
//             queue@0x40 + CMutex*@0x44 + @0x48 + saveInterval@0x4c（0x50）
class CAppThread : public CThreadInterface
{
public:
    CAppThread();
    virtual ~CAppThread();
    virtual void dispatch(void* param);
    void attach(CApplication* app, int idx);
    CApplication* m_app;             // +0xc
    CFrameCountHandler m_frame;      // +0x10
    std::queue<CUdpRecvBuffer*>* m_queue;  // +0x40（ORIG 为类型化指针：成员直装 esi/ebx，
                                           //  void* + 强转会产生 eax 中转）
    CMutex* m_lock;                  // +0x44
    int m_reserved;                  // +0x48
    int m_saveInterval;              // +0x4c
};

#endif // DNF_APP_THREAD_H_
