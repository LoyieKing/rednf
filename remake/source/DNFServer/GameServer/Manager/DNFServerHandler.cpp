// df_manager_r — CServerHandler（ORIG DNFServerHandler.cpp）
#include "DNFServerHandler.h"

#include <stdio.h>
#include <string.h>

#include "DNFApplication.h"
#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFTcpServerInterface.h"
#include "Peer.h"
#include "TcpNetSystem.h"

CServerHandler::CServerHandler() : m_app(0)
{
    m_pad = 0;
}
CServerHandler::~CServerHandler() {}

void CServerHandler::Attach(CApplication* app)
{
    if (app)
        m_app = app;
}

void CServerHandler::SetMonitorServerIpPort(unsigned char idx, unsigned int ip, unsigned short port) {}

void CServerHandler::SendToMonitorServer(char* buf, int len, unsigned char idx)
{
    CMonitorServer* p = &m_monitorServers[idx];
    p->SendToServer(buf, len);
}

void CServerHandler::Load(ST_ServerInfo* infos)
{
    unsigned char idx;
    int i;
    for (i = 0; i <= 0x64; i++)
    {
        if (infos[i].m_type == 3)
        {
            idx = infos[i].m_index;
            if (idx > 0x64)
                throw CDNFException("CServerHandler::Load() Server Table Exception Break!");
            m_monitorServers[idx].Init(infos[i].m_name, infos[i].m_port, idx);
        }
    }
}

CTcpServer* CServerHandler::GetTcpServer(unsigned int idx)
{
    for (std::map<unsigned int, CTcpServer*>::iterator it = m_tcpServers.begin();
         it != m_tcpServers.end(); ++it)
    {
        CTcpServer* server = it->second;
        if ((unsigned int)server->GetSocket() == idx)
            return server;
    }
    return 0;
}

CTcpServer* CServerHandler::GetTcpServer(unsigned char idx)
{
    std::map<unsigned int, CTcpServer*>::iterator it = m_tcpServers.find(idx);
    if (it != m_tcpServers.end())
        return it->second;
    return 0;
}

CMonitorServer* CServerHandler::GetMonitorServer(int idx)
{
    if (idx <= 0x64 && m_monitorServers[idx].IsValidMonitorServer())
        return m_monitorServers + idx;
    CMyFileLog log(__FUNCTION__, 0xc7);
    log("./log/Server.log", "Server Index Over Index : %d!\n", idx);
    return 0;
}

void CServerHandler::Process()
{
    CMonitorServer* p = m_monitorServers;
    int i = 0x65;
    while (i-- != 0)
    {
        if (!p->IsValidMonitorServer())
        {
            p++;
            continue;
        }
        if (p->IsConnected())
        {
            if (p->IsHeartBeatTimeOver())
            {
                p->OnDisconnect();
                register int index = 0x66 - i;
                CMyFileLog log(__FUNCTION__, 0x55);
                log("./log/MonitorDown", "CServerHandler::Process() Index : %d!\n", index);
            }
        }
        p++;
    }
    CheckTcpServerHeartbeat();
}

int CServerHandler::CreateTcpServer(unsigned char idx, unsigned int port)
{
    CTcpServer* server = new CTcpServer;
    server->Init(port, m_app->Get_TcpNetSystem());
    server->SetServerIndex(idx);
    if (m_tcpServers.insert(std::make_pair(idx, server)).second)
        return (int)server;
    delete server;
    return 0;
}

bool CServerHandler::DeleteTcpServer(unsigned char idx)
{
    std::map<unsigned int, CTcpServer*>::iterator it = m_tcpServers.find(idx);
    if (it != m_tcpServers.end())
    {
        delete it->second;
        m_tcpServers.erase(it);
        CMyFileLog(__FUNCTION__, 0x113)("./log/Tcp", "TcpMonitorServer Delete !");
        return 1;
    }
    return 0;
}

void CServerHandler::CheckTcpServerHeartbeat()
{
    for (std::map<unsigned int, CTcpServer*>::iterator it = m_tcpServers.begin();
         it != m_tcpServers.end(); ++it)
    {
        CTcpServer* server = it->second;
        if (server && server->IsHeartbeatTimeOver())
        {
            CPeer* peer = m_app->Get_TcpNetSystem()->GetPeer((unsigned int)server->GetSocket());
            if (peer)
            {
                peer->DisConnSig();
                m_app->Get_TcpNetSystem()->DeletePeer(peer);
            }
            m_tcpServers.erase(it);
            break;
        }
    }
}

void CServerHandler::SendAllTcpServer(PacketHeader* header)
{
    CTcpServer* server = 0;
    char* buf = 0;
    for (std::map<unsigned int, CTcpServer*>::iterator it = m_tcpServers.begin();
         it != m_tcpServers.end(); ++it)
    {
        server = it->second;
        if (server->IsValidServer())
        {
            buf = server->makePacketHeader(header->packetId, header->packetSize);
            memcpy(buf + 0xa, (char*)header + 0xa, header->packetSize - 0xaU);
            server->SendToServer(buf);
        }
    }
    __asm__ __volatile__("nop");
}

void CServerHandler::SetConnectFlag(unsigned char idx, bool flag)
{
    if (idx <= 0x64 && m_monitorServers[idx].IsValidMonitorServer())
    {
        m_monitorServers[idx].SetConnFlag(flag);
        return;
    }
    register int nIdx = idx;
    CMyFileLog log(__FUNCTION__, 0xa7);
    log("./log/Server.log", "Monitor Server Index Over Index : %d!\n", nIdx);
}

int CServerHandler::GetAlivedMonitorServer()
{
    if (m_tcpServers.empty())
        return 0;
    int count = 0;
    for (std::map<unsigned int, CTcpServer*>::iterator it = m_tcpServers.begin();
         it != m_tcpServers.end(); ++it)
    {
        CTcpServer* server = it->second;
        if (server->IsValidServer())
            count++;
    }
    return count;
}

bool CServerHandler::IsConnectedMonitorServer(unsigned char idx)
{
    if (idx <= 0x64 && m_monitorServers[idx].IsValidMonitorServer())
        return m_monitorServers[idx].IsConnected();
    register int nIdx = idx;
    CMyFileLog log(__FUNCTION__, 0x91);
    log("./log/Server.log", "Server Index Over Index : %d!\n", nIdx);
    return 0;
}

void CServerHandler::ResetHeartBeat(unsigned char idx)
{
    if (idx <= 0x64 && m_monitorServers[idx].IsValidMonitorServer())
    {
        m_monitorServers[idx].ResetHeartBeat();
        return;
    }
    register int nIdx = idx;
    CMyFileLog log(__FUNCTION__, 0x70);
    log("./log/Server.log", "Server Index Over Index : %d!\n", nIdx);
}

void CServerHandler::SendToTcpServer(PacketHeader* header, unsigned char idx)
{
    CTcpServer* server = GetTcpServer(idx);
    char* buf = 0;
    if (server)
    {
        buf = server->makePacketHeader(header->packetId, header->packetSize);
        memcpy(buf + 0xa, (char*)header + 0xa, header->packetSize - 0xaU);
        server->SendToServer(buf);
    }
}

void CServerHandler::SendToTcpServer(char* buf, int len, unsigned char idx)
{
    CTcpServer* server = GetTcpServer(idx);
    if (server)
        server->SendToServer(buf);
}

void CServerHandler::SendAllToMonitorServer(char* buf, int len)
{
    CMonitorServer* p = m_monitorServers;
    int i = 0x65;
    while (i-- != 0)
    {
        if (p->IsValidMonitorServer() && p->IsConnected())
            p->SendToServer(buf, len);
        p++;
    }
}
