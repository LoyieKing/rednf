// df_monitor_r — DNFTcpHandler（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "DNFTcpHandler.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "TcpNetSystem.h"

void* CTcpHandler::GetEventPtr(int idx)
{
    if (m_epoll == 0)
    {
        return 0;
    }
    return m_epoll->GetEventPtr(idx);
}

int CTcpHandler::SetPeer(void* peer, int fd, bool flag)
{
    if (m_epoll == 0)
    {
        return -1;
    }
    return m_epoll->SetEpoll(peer, fd, flag);
}

int CTcpHandler::WaitForEvent()
{
    if (m_epoll == 0)
    {
        return -1;
    }
    return ((EpollHandler*)m_epoll)->WaitForEvent();
}

int CTcpHandler::ResetEpoll(int flag)
{
    if (m_epoll == 0)
    {
        return -1;
    }
    int ret = m_epoll->ResetEpoll(flag);
    return ret;
}

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

char CTcpHandler::IsSetInEvent(int idx)
{
    if (m_epoll == 0)
    {
        return 0;
    }
    return m_epoll->IsSetInEvent(idx);
}

char CTcpHandler::IsSetOutEvent(int idx)
{
    if (m_epoll == 0)
    {
        return 0;
    }
    return m_epoll->IsSetOutEvent(idx);
}

char CTcpHandler::IsSetErrEvent(int idx)
{
    if (m_epoll == 0)
    {
        return 0;
    }
    return m_epoll->IsSetErrEvent(idx);
}
