// df_monitor_r — DNFDBServer（从 MonitorTypes/App/Table 拆分）
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

#include "DNFDBServer.h"
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

CDBServer::CDBServer() {}

CDBServer::CDBServer(stServerInfo* info) : CServerInterface(info) {}

CDBServer::~CDBServer() {}

bool CDBServer::Initialize()
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

bool CDBServer::Destroy()
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

CTcpDBServer::CTcpDBServer()
{
    m_port = 0;
    m_sock = -1;
    m_net = 0;
    m_ip.clear();
}

CTcpDBServer::~CTcpDBServer() { Clear(); }

void CTcpDBServer::Clear()
{
    m_sock = -1;
    m_net = 0;
    m_port = 0;
    m_ip.clear();
}

void CTcpDBServer::Init(CTcpNetSystem* net) { m_net = net; }

void CTcpDBServer::SetIP(std::string ip) {}

void CTcpDBServer::SetPort(unsigned short port) { m_port = port; }

int* CTcpDBServer::GetSockRef() { return &m_sock; }

int CTcpDBServer::GetSock() { return m_sock; }

char CTcpDBServer::IsValidServer()
{
    if (m_sock != -1 && m_net != 0)
    {
        return 1;
    }
    CMyFileLog log("IsValidServer", 0x16d);
    log("./log/TcpServer", "Invalid Tcp Server(%d,%x)", m_sock, m_net);
    return 0;
}

const char* CTcpDBServer::GetIP() { return m_ip.c_str(); }

unsigned short CTcpDBServer::GetPort() { return m_port; }

void CTcpDBServer::SendHeartbeat()
{
    char* buf = makePacketHeader(0x106a, 0xb);
    if (buf != 0)
    {
        buf[10] = 0xa;
        SendToServer(buf);
    }
}

void CTcpDBServer::SendLogin()
{
    char* buf = makePacketHeader(0x1068, 0xb);
    if (buf != 0)
    {
        buf[10] = 0xa;
        SendToServer(buf);
    }
}

void CTcpDBServer::SendLogout()
{
    char* buf = makePacketHeader(0x1069, 0xb);
    if (buf != 0)
    {
        buf[10] = 0xa;
        SendToServer(buf);
    }
}

void CTcpDBServer::Connected()
{
    SendLogin();
}

void CTcpDBServer::DisConnected()
{
    m_sock = -1;
}

char* CTcpDBServer::makePacketHeader(unsigned short id, unsigned short size)
{
    if (m_net == 0)
    {
        return 0;
    }
    char* buf = (char*)((CTcpNetSystem*)m_net)->Acquire_TcpSendBuffer();
    *(unsigned short*)buf = id;
    *(unsigned short*)(buf + 2) = size;
    *(unsigned int*)(buf + 6) = (unsigned int)m_sock;
    return buf;
}

void CTcpDBServer::SendToServer(char* buf)
{
    ((CTcpNetSystem*)m_net)->PushTcpSendPacketQ(buf);
}
