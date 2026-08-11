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

CServerHandler::CServerHandler() {}
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
    for (int i = 0; i <= 0x64; i++)
    {
        if (infos[i].m_type == 3)
        {
            unsigned char idx = infos[i].m_index;
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
        return (CMonitorServer*)((char*)this + idx * 0x14);
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
                CMyFileLog log(__FUNCTION__, 0x55);
                log("./log/MonitorDown", "CServerHandler::Process() Index : %d!\n", 0x66 - i);
            }
        }
        p++;
    }
    CheckTcpServerHeartbeat();
}

char CServerHandler::CreateTcpServer(unsigned char idx, unsigned int port)
{
    CTcpServer* server = new CTcpServer;
    server->Init(port, m_app->Get_TcpNetSystem());
    server->SetServerIndex(idx);
    std::pair<std::map<unsigned int, CTcpServer*>::iterator, bool> pr =
        m_tcpServers.insert(std::make_pair(idx, server));
    if (pr.second)
        return 1;
    delete server;
    return 0;
}

char CServerHandler::DeleteTcpServer(unsigned char idx)
{
    std::map<unsigned int, CTcpServer*>::iterator it = m_tcpServers.find(idx);
    if (it != m_tcpServers.end())
    {
        CTcpServer* server = it->second;
        if (server)
            delete server;
        m_tcpServers.erase(it);
        CMyFileLog log("DeleteTcpServer", 0x113);
        log("./log/Tcp", "TcpMonitorServer Delete !");
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
    for (std::map<unsigned int, CTcpServer*>::iterator it = m_tcpServers.begin();
         it != m_tcpServers.end(); ++it)
    {
        CTcpServer* server = it->second;
        if (server->IsValidServer())
        {
            char* buf = (char*)server->makePacketHeader(header->packetId, header->packetSize);
            memcpy(buf + 0xa, (char*)header + 0xa, header->packetSize - 0xa);
            server->SendToServer(buf);
        }
    }
}

void CServerHandler::SetConnectFlag(unsigned char idx, bool flag)
{
    if (idx <= 0x64 && m_monitorServers[idx].IsValidMonitorServer())
    {
        m_monitorServers[idx].SetConnFlag(flag);
        return;
    }
    CMyFileLog log(__FUNCTION__, 0xa7);
    log("./log/Server.log", "Monitor Server Index Over Index : %d!\n", idx);
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

char CServerHandler::IsConnectedMonitorServer(unsigned char idx)
{
    if (idx <= 0x64 && m_monitorServers[idx].IsValidMonitorServer())
        return m_monitorServers[idx].IsConnected();
    CMyFileLog log(__FUNCTION__, 0x91);
    log("./log/Server.log", "Server Index Over Index : %d!\n", idx);
    return 0;
}

void CServerHandler::ResetHeartBeat(unsigned char idx)
{
    if (idx <= 0x64 && m_monitorServers[idx].IsValidMonitorServer())
    {
        m_monitorServers[idx].ResetHeartBeat();
        return;
    }
    CMyFileLog log(__FUNCTION__, 0x70);
    log("./log/Server.log", "Server Index Over Index : %d!\n", idx);
}

void CServerHandler::SendToTcpServer(PacketHeader* header, unsigned char idx)
{
    CTcpServer* server = GetTcpServer(idx);
    if (!server)
        return;
    char* buf = (char*)server->makePacketHeader(header->packetId, header->packetSize);
    memcpy(buf + 0xa, (char*)header + 0xa, header->packetSize - 0xa);
    server->SendToServer(buf);
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
