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

// ORIG 内部 Tcp 服务器登录/心跳包：id@0 size@2 pad@4 sock@6 flag@0xa（packed 11B）
struct __attribute__((packed)) TcpServerPkt
{
    unsigned short id;
    unsigned short size;
    unsigned short pad;
    unsigned int sock;
    unsigned char flag;
};

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

void CTcpDBServer::Init(CTcpNetSystem* net) { this->m_net = net; }

// [2026-09-14 修复] ORIG df_monitor_r `CTcpDBServer::SetIP(std::string)`（0x080664bc，0x1a 字节）
//   本地是空函数体（func_size_audit.py 用 32B 阈值时**漏掉**了它，因为它只有 0x1a）。
//   ORIG 反汇编：`mov eax,[ebp+8]; mov edx,[ebp+0xc]; mov [esp+4],edx; mov [esp],eax;
//   call std::string::operator=` ⇒ 就是 `m_ip = ip;`。
//   后果（整机 A/B 实证）：`m_tcpDbServer.SetIP()` 变成 no-op ⇒ `m_ip` 永远是空串 ⇒
//   `CServerHandler::Process()` 里 `if (*GetIP()=='\0' || GetPort()==0) {} else { 重连 }`
//   永远走空分支 ⇒ **monitor 与 DBMW(20203) 的连接断掉后永不重连**，
//   `CTcpDBServer::IsValidServer()` 每轮都判无效并打日志：
//   ORIG 轮 `TcpServer` 5 行（含 1 行 "try connect to DBMW(127.0.0.1, 20203)"），
//   我们轮 144 行（其中 141 行 "Invalid Tcp Server"），两轮各自完全可复现。
void CTcpDBServer::SetIP(std::string ip) { m_ip = ip; }

void CTcpDBServer::SetPort(unsigned short port) { m_port = port; }

int* CTcpDBServer::GetSockRef() { return &m_sock; }

int CTcpDBServer::GetSock() { return m_sock; }

bool CTcpDBServer::IsValidServer()
{
    if (m_sock != -1 && m_net != 0)
    {
        return 1;
    }
    register CTcpNetSystem* net = m_net;
    register int sock = m_sock;
    CMyFileLog log(__FUNCTION__, 0x16d);
    log("./log/TcpServer", "Invalid Tcp Server(%d,%x)", sock, net);
    return 0;
}

const char* CTcpDBServer::GetIP() { return m_ip.c_str(); }

unsigned short CTcpDBServer::GetPort() { return m_port; }

void CTcpDBServer::SendHeartbeat()
{
    char* buf = makePacketHeader(0x106a, 0xb);
    TcpServerPkt* p = (TcpServerPkt*)buf;
    if (p != 0)
    {
        p->flag = 0xa;
        SendToServer((char*)p);
    }
}

void CTcpDBServer::SendLogin()
{
    char* buf = makePacketHeader(0x1068, 0xb);
    TcpServerPkt* p = (TcpServerPkt*)buf;
    if (p != 0)
    {
        p->flag = 0xa;
        SendToServer((char*)p);
    }
}

void CTcpDBServer::SendLogout()
{
    char* buf = makePacketHeader(0x1069, 0xb);
    TcpServerPkt* p = (TcpServerPkt*)buf;
    if (p != 0)
    {
        p->flag = 0xa;
        SendToServer((char*)p);
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
    char* buf;
    TcpServerPkt* puVar1;
    if (m_net != 0)
    {
        buf = (char*)m_net->Acquire_TcpSendBuffer();
        puVar1 = (TcpServerPkt*)buf;
        puVar1->id = id;
        puVar1->size = size;
        puVar1->sock = (unsigned int)m_sock;
        return (char*)puVar1;
    }
    return 0;
}

void CTcpDBServer::SendToServer(char* buf)
{
    m_net->PushTcpSendPacketQ(buf);
}
