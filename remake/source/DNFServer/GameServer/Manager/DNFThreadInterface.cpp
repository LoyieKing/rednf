// df_manager_r — CThreadInterface（ORIG DNFThreadInterface.cpp）
#include "DNFThreadInterface.h"

#include <stdio.h>
#include <pthread.h>

CThreadInterface::CThreadInterface()
{
    m_thread = 0;
    m_stop = 0;
}

CThreadInterface::~CThreadInterface() {}

bool CThreadInterface::begin()
{
    // ORIG 直接传成员函数地址（&CThreadInterface::dispatch_proxy，pthread 以
    // param 作为 this 调用，与 thread_proxy 包装等价）。
    int ret = pthread_create(&this->m_thread, 0,
                             (void* (*)(void*))&CThreadInterface::dispatch_proxy, (void*)this);
    if (ret < 0)
    {
        puts("[ThreadInterface::begin] Can't begin thread");
        return 0;
    }
    return 1;
}

void* CThreadInterface::dispatch_proxy(void* param)
{
    CThreadInterface* t = (CThreadInterface*)param;
    t->dispatch(param);
    return 0;
}

void CThreadInterface::join()
{
    pthread_join(m_thread, 0);
}

void CThreadInterface::stop() { m_stop = 0; }
