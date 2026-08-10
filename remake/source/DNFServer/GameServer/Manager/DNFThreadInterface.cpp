// df_manager_r — CThreadInterface（ORIG DNFThreadInterface.cpp）
#include "DNFThreadInterface.h"

#include <stdio.h>
#include <pthread.h>

static void* thread_proxy(void* param)
{
    return ((CThreadInterface*)param)->dispatch_proxy(param);
}

CThreadInterface::CThreadInterface()
{
    m_thread = 0;
    m_stop = 0;
}

CThreadInterface::~CThreadInterface() {}

char CThreadInterface::begin()
{
    int ret = pthread_create(&m_thread, 0, thread_proxy, this);
    if (ret < 0)
    {
        puts("[ThreadInterface::begin] Can't begin thread");
        return 0;
    }
    return 1;
}

void* CThreadInterface::dispatch_proxy(void* param)
{
    return dispatch(param);
}

void CThreadInterface::join()
{
    pthread_join(m_thread, 0);
}

void CThreadInterface::stop() { m_stop = 0; }
