// df_monitor_r — DNFGameServer（从 MonitorTypes/App/Table 拆分）
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

#include "DNFGameServer.h"
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
#include "TcpNetSystem.h"

CGameServer::CGameServer() { m_socket = 0; }

CGameServer::CGameServer(stServerInfo* info) : CServerInterface(info) { m_socket = 0; }

CGameServer::~CGameServer() {}

bool CGameServer::Initialize()
{
    if (!CServerInterface::Initialize())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

bool CGameServer::Destroy()
{
    if (!CServerInterface::Destroy())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

unsigned int CGameServer::GetSocket() { return m_socket; }

void CGameServer::SetSocket(unsigned int sock)
{
    m_socket = sock;
}

CTcpGameServer::CTcpGameServer()
{
    m_sock = 0;
    m_net = 0;
    m_channelNo = 0;
    m_channelType = 0;
}

CTcpGameServer::~CTcpGameServer() {}

char* CTcpGameServer::makePacketHeader(unsigned short id, unsigned short size)
{
    if (m_net == 0)
    {
        return 0;
    }
    char* buf = (char*)m_net->Acquire_TcpSendBuffer();
    *(unsigned short*)buf = id;
    *(unsigned short*)(buf + 2) = size;
    *(unsigned int*)(buf + 6) = (unsigned int)m_sock;
    return buf;
}

void CTcpGameServer::SendToGameServer(char* buf)
{
    if (m_net != 0)
    {
        m_net->PushTcpSendPacketQ(buf);
    }
}

void CTcpGameServer::SendToGameServer(PacketHeader* pkt)
{
    char* buf = makePacketHeader(*(unsigned short*)pkt, *(unsigned short*)((char*)pkt + 2));
    if (buf != 0)
    {
        memcpy(buf + 10, (char*)pkt + 10, *(unsigned short*)((char*)pkt + 2) - 10);
        SendToGameServer(buf);
    }
}

void CTcpGameServer::SetChannelType(int type)
{
    m_channelType = type;
}

int CTcpGameServer::GetChannelType()
{
    return m_channelType;
}

void CTcpGameServer::Init(unsigned int sock, CTcpNetSystem* net)
{
    m_sock = sock;
    m_net = net;
}

void CTcpGameServer::SetChannelNo(unsigned char channel)
{
    m_channelNo = channel;
}

unsigned char CTcpGameServer::GetChannelNo()
{
    return m_channelNo;
}

char CTcpGameServer::IsValidServer()
{
    if (m_sock != 0 && m_net != 0)
    {
        return 1;
    }
    return 0;
}
