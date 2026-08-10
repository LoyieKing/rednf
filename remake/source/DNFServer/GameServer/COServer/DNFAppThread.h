#ifndef DNF_APP_THREAD_H_
#define DNF_APP_THREAD_H_

#include "DNFTickHandler.h"
#include "DNFThreadInterface.h"

class CApplication;

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
    void* m_queue;                   // +0x40
    void* m_lock;                    // +0x44
    int m_reserved;                  // +0x48
    int m_saveInterval;              // +0x4c
};

#endif // DNF_APP_THREAD_H_
