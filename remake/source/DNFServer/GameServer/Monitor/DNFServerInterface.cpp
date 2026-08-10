// df_monitor_r — DNFServerInterface（从 MonitorTypes/App/Table 拆分）
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
    m_field8[0] = 0;
    m_field8[1] = 0;
    m_field8[2] = 0;
    m_udpHandler = 0;
}

CServerInterface::CServerInterface(stServerInfo* info)
{
    m_info = info;
    m_field8[0] = 0;
    m_field8[1] = 0;
    m_field8[2] = 0;
    m_udpHandler = 0;
}

CServerInterface::~CServerInterface() {}

stServerInfo* CServerInterface::GetServerInfo() { return m_info; }

void* CServerInterface::GetUdpHandler() { return m_udpHandler; }

unsigned char CServerInterface::GetGroupNo() { return *(unsigned char*)((char*)m_info); }

void CServerInterface::SetServerInfo(stServerInfo* info) { m_info = info; }

bool CServerInterface::Initialize()
{
    if (m_udpHandler != 0)
    {
        return false;
    }
    m_udpHandler = new CUdpHandler;
    ((CUdpHandler*)m_udpHandler)->InitClientSocket();
    return true;
}

bool CServerInterface::Destroy()
{
    if (m_udpHandler != 0)
    {
        ::operator delete(m_udpHandler);
    }
    return true;
}

bool CServerInterface::IsValidServer() { return 1; }

char CServerInterface::IsConnected() { return 1; }

char CServerInterface::IsHeartBeatTimeOver()
{
    m_field8[1] = m_field8[1] - 1;
    if (m_field8[1] == 0)
    {
        m_field8[2] = m_field8[2] + 1;
        if (m_field8[2] > 20)
        {
            return 1;
        }
        m_field8[1] = 20;
    }
    return 0;
}

unsigned char CServerInterface::GetChannelNo() { return 0; }

void CServerInterface::OnDisconnect()
{
    m_field8[0] = 0;
    m_field8[1] = 20;
    m_field8[2] = 0;
}

void CServerInterface::SetConnFlag(bool flag)
{
    *(char*)((char*)this + 8) = (char)flag;
}

void CServerInterface::ResetHeartBeat()
{
    *(char*)((char*)this + 9) = 0x14;
    *(char*)((char*)this + 10) = 0;
}

int CServerInterface::SendToServer(char* buf, int len)
{
    if (*(int*)((char*)this + 0xc) == 0)
    {
        return 0;
    }
    return ((CUdpHandler*)*(int*)((char*)this + 0xc))
        ->SendToServer(buf, len, *(unsigned short*)(*(int*)((char*)this + 4) + 0x14),
                       (char*)(*(int*)((char*)this + 4) + 3));
}

