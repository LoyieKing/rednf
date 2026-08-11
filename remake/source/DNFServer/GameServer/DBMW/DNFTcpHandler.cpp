// df_dbmw_r - DNFTcpHandler (ORIG DNFTcpHandler.cpp)
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

CTcpHandler::CTcpHandler()
{
    m_epoll = new EpollHandler;
}
CTcpHandler::~CTcpHandler()
{
    if (m_epoll)
    {
        delete m_epoll;
        m_epoll = 0;
    }
}
int CTcpHandler::WaitForEvent()
{
    if (!m_epoll)
        return -1;
    return m_epoll->WaitForEvent();
}
int CTcpHandler::ResetEpoll(int flag)
{
    if (!m_epoll)
        return -1;
    int nRet = m_epoll->ResetEpoll(flag);
    return nRet;
}
int CTcpHandler::SetPeer(void* peer, int fd, bool flag)
{
    if (!m_epoll)
        return -1;
    return m_epoll->SetEpoll(peer, fd, flag);
}
void* CTcpHandler::GetEventPtr(int idx)
{
    if (!m_epoll)
        return 0;
    return m_epoll->GetEventPtr(idx);
}
char CTcpHandler::IsSetInEvent(int idx)
{
    if (!m_epoll)
        return 0;
    return m_epoll->IsSetInEvent(idx);
}
char CTcpHandler::IsSetOutEvent(int idx)
{
    if (!m_epoll)
        return 0;
    return m_epoll->IsSetOutEvent(idx);
}
char CTcpHandler::IsSetErrEvent(int idx)
{
    if (!m_epoll)
        return 0;
    return m_epoll->IsSetErrEvent(idx);
}
