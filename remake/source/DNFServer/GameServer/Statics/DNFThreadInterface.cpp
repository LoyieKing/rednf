// df_statics_r — DNFThreadInterface（ORIG DNFThreadInterface.o 拆分）
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
bool CThreadInterface::begin()
{
    int r = pthread_create(&m_handle, 0, dispatch_proxy, this);
    if (-1 >= r)
    {
        puts("[ThreadInterface::begin] Can't begin thread");
    }
    return -1 < r;
}
void CThreadInterface::stop()
{
    m_running = 0;
}
void CThreadInterface::join()
{
    pthread_join(m_handle, 0);
}
void* CThreadInterface::dispatch_proxy(void* temp)
{
    ((CThreadInterface*)temp)->dispatch(temp);
    return 0;
}
