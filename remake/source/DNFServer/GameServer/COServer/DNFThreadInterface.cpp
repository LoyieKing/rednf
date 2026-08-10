// df_coserver_r — CThreadInterface（ORIG DNFThreadInterface.cpp）
#include <pthread.h>
#include <stdio.h>

#include "DNFThreadInterface.h"

CThreadInterface::CThreadInterface()
{
    m_handle = 0;
    m_running = 0;
}

CThreadInterface::~CThreadInterface()
{
}

void CThreadInterface::stop()
{
    m_running = 0;
}

void CThreadInterface::join()
{
    pthread_join(m_handle, 0);
}

bool CThreadInterface::begin()
{
    int r = pthread_create(&m_handle, 0, dispatch_proxy, this);
    if (r < 0)
    {
        puts("[ThreadInterface::begin] Can't begin thread");
        return false;
    }
    return true;
}

void* CThreadInterface::dispatch_proxy(void* temp)
{
    CThreadInterface* th = (CThreadInterface*)temp;
    th->dispatch(temp);
    return 0;
}
