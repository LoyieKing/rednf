// df_dbmw_r - DNFThreadInterface (ORIG DNFThreadInterface.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

CThreadInterface::CThreadInterface()
{
    m_thread = 0;
    m_stop = 0;
}
CThreadInterface::~CThreadInterface() {}
static void* thread_proxy(void* param)
{
    return ((CThreadInterface*)param)->dispatch_proxy(param);
}
bool CThreadInterface::begin()
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
    dispatch(param);
    return 0;
}
void CThreadInterface::join()
{
    pthread_join(m_thread, 0);
}
void CThreadInterface::stop() { m_stop = 0; }
