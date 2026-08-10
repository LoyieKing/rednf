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
    return m_epoll ? m_epoll->WaitForEvent() : -1;
}
int CTcpHandler::ResetEpoll(int flag)
{
    return m_epoll ? m_epoll->ResetEpoll(flag) : -1;
}
int CTcpHandler::SetPeer(void* peer, int fd, bool flag)
{
    return m_epoll ? m_epoll->SetEpoll(peer, fd, flag) : -1;
}
void* CTcpHandler::GetEventPtr(int idx)
{
    return m_epoll ? m_epoll->GetEventPtr(idx) : 0;
}
char CTcpHandler::IsSetInEvent(int idx)
{
    return m_epoll ? m_epoll->IsSetInEvent(idx) : 0;
}
char CTcpHandler::IsSetOutEvent(int idx)
{
    return m_epoll ? m_epoll->IsSetOutEvent(idx) : 0;
}
char CTcpHandler::IsSetErrEvent(int idx)
{
    return m_epoll ? m_epoll->IsSetErrEvent(idx) : 0;
}
