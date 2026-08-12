// df_monitor_r — DNFManagerServer（从 MonitorTypes/App/Table 拆分）
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

#include "DNFManagerServer.h"
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
#include "DNFPacketTranslater.h"
#include "DNFUdpHandler.h"
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

CExchangeServer::CExchangeServer() { m_active = 0; }

CExchangeServer* GetInstanceExchangeServer()
{
    static CExchangeServer instance;
    return &instance;
}

unsigned int CExchangeServer::GetExchangeServerIp()
{
    return m_ip;
}

unsigned short CExchangeServer::GetExchangeServerPort()
{
    return m_port;
}

unsigned int CExchangeServer::GetExchangeServerChannelNo()
{
    return m_code;
}

void CExchangeServer::SetExchageServer(unsigned int ip, short port, int code, bool& result)
{
    time_t now = time(0);
    in_addr ipLocal;
    ipLocal.s_addr = ip;
    in_addr oldIp;
    oldIp.s_addr = ((RA_UINT<8>*)this)->v;
    result = false;
    if (m_active != 0)
    {
        if (m_ip == ip && m_port == port && m_code == code)
        {
            if (now - m_time < 0x1f)
            {
                result = true;
            }
            m_time = now;
        }
        else if (0x1e < now - m_time)
        {
            DNF_LOG_SCOPE_LINE(0xe21,"./log/ExchangeServer",
                "timeout : new(%s,%d,%d,%d) old(%s,%d,%d,%d)", inet_ntoa(ipLocal), port,
                code, now, inet_ntoa(oldIp), m_port, m_code, (int)m_time);
            m_ip = ip;
            m_port = port;
            m_code = code;
            m_time = now;
        }
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0xe2c,"./log/ExchangeServer", "insert new(%s,%d,%d,%d)", inet_ntoa(ipLocal), port,
            code, now);
        m_ip = ip;
        m_port = port;
        m_code = code;
        m_time = now;
        m_active = 1;
    }
}

CManagerServer::CManagerServer() {}

CManagerServer::CManagerServer(stServerInfo* info) : CServerInterface(info) {}

CManagerServer::~CManagerServer() {}

bool CManagerServer::Initialize()
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

bool CManagerServer::Destroy()
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

void CManagerServer::SendHeartBeat(int group)
{
    if (GetUdpHandler() != 0)
    {
        Packet_Monitor_UDP_HeartBeat pkt;
        pkt.m_fieldA = (char)group;
        ((CUdpHandler*)GetUdpHandler())
            ->SendToServer((char*)&pkt, 0xb, GetServerInfo()->m_port,
                           GetServerInfo()->m_name);
    }
}

CTcpManagerServer::CTcpManagerServer()
{
    m_port = 0;
    m_sock = -1;
    m_net = 0;
    m_ip.clear();
}

CTcpManagerServer::~CTcpManagerServer() { Clear(); }

void CTcpManagerServer::Clear()
{
    m_sock = -1;
    m_net = 0;
    m_port = 0;
    m_ip.clear();
}

void CTcpManagerServer::Init(CTcpNetSystem* net)
{
    this->m_net = net;
}

void CTcpManagerServer::SetIP(std::string ip) {}

void CTcpManagerServer::SetPort(unsigned short port)
{
    m_port = port;
}

int* CTcpManagerServer::GetSockRef() { return &m_sock; }

int CTcpManagerServer::GetSock() { return m_sock; }

char CTcpManagerServer::IsValidServer()
{
    if (m_sock != -1 && m_net != 0)
    {
        return 1;
    }
    register CTcpNetSystem* net = m_net;
    register int sock = m_sock;
    CMyFileLog log(__FUNCTION__, 0x1a0);
    log("./log/TcpServer", "Invalid Tcp Server(%d,%x)", sock, net);
    return 0;
}

const char* CTcpManagerServer::GetIP() { return m_ip.c_str(); }

unsigned short CTcpManagerServer::GetPort() { return m_port; }

void CTcpManagerServer::SendHeartbeat(unsigned char group)
{
    char* buf = makePacketHeader(0x106d, 0xb);
    TcpServerPkt* p = (TcpServerPkt*)buf;
    if (p != 0)
    {
        p->flag = (unsigned char)group;
        SendToServer((char*)p);
    }
}

void CTcpManagerServer::SendLogin(unsigned char group)
{
    char* buf = makePacketHeader(0x106b, 0xb);
    TcpServerPkt* p = (TcpServerPkt*)buf;
    if (p != 0)
    {
        p->flag = (unsigned char)group;
        SendToServer((char*)p);
    }
}

void CTcpManagerServer::SendLogout()
{
    char* buf = makePacketHeader(0x106c, 0xb);
    TcpServerPkt* p = (TcpServerPkt*)buf;
    if (p != 0)
    {
        p->flag = 0xb;
        SendToServer((char*)p);
    }
}

void CTcpManagerServer::Connected(unsigned char group)
{
    SendLogin(group);
}

void CTcpManagerServer::DisConnected()
{
    m_sock = -1;
}

char* CTcpManagerServer::makePacketHeader(unsigned short id, unsigned short size)
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

void CTcpManagerServer::SendToServer(char* buf)
{
    m_net->PushTcpSendPacketQ(buf);
}

void CTcpManagerServer::SendTcpPacket(PacketHeader* pkt)
{
    char* buf = makePacketHeader(*(unsigned short*)pkt, ((RA_U16<2>*)pkt)->v);
    if (buf != 0)
    {
        memcpy(buf + 10, (char*)pkt + 10, (int)(((RA_U16<2>*)pkt)->v - 10));
        SendToServer(buf);
    }
}

namespace exchange_server
{
CCacheCharacterMgr::CCacheCharacterMgr()
{
}

CCacheCharacterMgr::~CCacheCharacterMgr() {}

int CCacheCharacterMgr::CacheCharacter(unsigned int dbid, CACHE_CHARACTER_TYPE* type)
{
    type->m_field8 = (long)time(0);
    std::pair<std::map<unsigned int, CACHE_CHARACTER_TYPE>::iterator, bool> r =
        m_cache.insert(std::make_pair(dbid, *type));
    if (r.second)
    {
        if (m_cache.size() > 49999)
        {
            if (!CollectGarbage())
            {
                return 0;
            }
        }
    }
    else
    {
        r.first->second = *type;
    }
    CCacheCharacterTime t;
    t.m_time = (int)type->m_field8;
    t.m_charNo = (int)dbid;
    m_timeQueue.push(t);
    if (m_timeQueue.size() > 49999)
    {
        CollectGarbage();
    }
    return 1;
}

bool CCacheCharacterMgr::GetCacheCharacter(unsigned int dbid, CACHE_CHARACTER_TYPE* out)
{
    std::map<unsigned int, CACHE_CHARACTER_TYPE>::iterator it = m_cache.find(dbid);
    if (it != m_cache.end())
    {
        *out = it->second;
        m_cache.erase(it);
        if (time(0) - out->m_field8 > 0x1d)
        {
            return 0;
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

bool CCacheCharacterMgr::CollectGarbage()
{
    bool result = 0;
    CCacheCharacterTime top;
    time_t now = time(0);
    while (!m_timeQueue.empty())
    {
        top = m_timeQueue.top();
        if (now - top.m_time <= 0x1d)
        {
            return result;
        }
        std::map<unsigned int, CACHE_CHARACTER_TYPE>::iterator it =
            m_cache.find((unsigned int)top.m_charNo);
        if (it != m_cache.end() && 0x1d < now - it->second.m_field8)
        {
            m_cache.erase(it);
            result = 1;
        }
        m_timeQueue.pop();
    }
    return result;
}

void CCacheCharacterMgr::Reset()
{
    m_cache.clear();
    while (!m_timeQueue.empty())
    {
        m_timeQueue.pop();
    }
}

CCacheCharacterMgr* GetInstanceCacheCharacterMgr()
{
    static CCacheCharacterMgr instance;
    return &instance;
}
}
