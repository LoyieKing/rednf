// df_monitor_r — DNFServerInterface（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include "RawAccess.h"
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

#include "DNFServerInterface.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFAppConfig.h"
#include "DNFUdpHandler.h"

CServerInterface::CServerInterface()
{
    m_info = 0;
    m_connected = 0;
    m_heart = 0;
    m_padA[0] = 0;
    m_udpHandler = 0;
}

CServerInterface::CServerInterface(stServerInfo* info)
{
    m_info = info;
    m_connected = 0;
    m_heart = 0;
    m_padA[0] = 0;
    m_udpHandler = 0;
}

CServerInterface::~CServerInterface() {}

stServerInfo* CServerInterface::GetServerInfo() { return m_info; }

void* CServerInterface::GetUdpHandler() { return m_udpHandler; }

unsigned char CServerInterface::GetGroupNo() { return *(unsigned char*)((char*)m_info); }

void CServerInterface::SetServerInfo(stServerInfo* info) { m_info = info; }

bool CServerInterface::Initialize()
{
    if (m_udpHandler == 0)
    {
        m_udpHandler = new CUdpHandler;
        ((CUdpHandler*)m_udpHandler)->InitClientSocket();
        return 1;
    }
    return 0;
}

bool CServerInterface::Destroy()
{
    if (m_udpHandler != 0)
    {
        ::operator delete(m_udpHandler);
    }
    return true;
}

bool CServerInterface::IsValidServer()
{
    if (m_info->m_field0 == 0xff)
    {
        return 0;
    }
    return 1;
}

bool CServerInterface::IsConnected() { return m_connected; }

char CServerInterface::IsHeartBeatTimeOver()
{
    m_heart = m_heart - 1;
    if (m_heart == 0)
    {
        ++m_padA[0];
        if (m_padA[0] > 0x14)
        {
            return 1;
        }
        m_heart = 0x14;
    }
    return 0;
}

unsigned char CServerInterface::GetChannelNo() { return m_info->m_field1; }

void CServerInterface::OnDisconnect()
{
    m_connected = 0;
    m_heart = 20;
    m_padA[0] = 0;
}

void CServerInterface::SetConnFlag(bool flag)
{
    m_connected = flag;
}

void CServerInterface::ResetHeartBeat()
{
    m_heart = 0x14;
    m_padA[0] = 0;
}

int CServerInterface::SendToServer(char* buf, int len)
{
    if (m_udpHandler != 0)
    {
        return ((CUdpHandler*)m_udpHandler)
            ->SendToServer(buf, len, m_info->m_port,
                           (char*)((unsigned int)m_info + 3));
    }
    return 0;
}
