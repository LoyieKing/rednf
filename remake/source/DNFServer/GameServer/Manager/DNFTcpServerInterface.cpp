// df_manager_r — CTcpServer（ORIG DNFTcpServerInterface.cpp）
#include "DNFTcpServerInterface.h"

#include <stdio.h>
#include <time.h>

#include "DNFPacketBuffer.h"
#include "TcpNetSystem.h"

CTcpServer::CTcpServer()
{
    m_index = 0;
    m_socket = 0;
    m_net = 0;
    m_heartbeat = 0;
}

CTcpServer::~CTcpServer()
{
    m_socket = 0;
    m_net = 0;
    m_index = 0;
    m_heartbeat = 0;
}

void CTcpServer::Init(unsigned int sock, CTcpNetSystem* net)
{
    m_socket = sock;
    m_net = net;
}

bool CTcpServer::IsValidServer()
{
    return m_socket != 0 && m_net != 0;
}

char CTcpServer::IsHeartbeatTimeOver()
{
    time_t now;
    time(&now);
    if (m_heartbeat && now - m_heartbeat > 0x3b)
        return 1;
    return 0;
}

void CTcpServer::SendToServer(char* buf)
{
    m_net->PushTcpSendPacketQ(buf);
}

void CTcpServer::SetServerIndex(unsigned char idx) { m_index = idx; }
unsigned char CTcpServer::GetServerIndex() { return m_index; }
void* CTcpServer::GetSocket() { return (void*)m_socket; }
void CTcpServer::NotifyHeartbeat() { time(&m_heartbeat); }

char* CTcpServer::makePacketHeader(unsigned short type, unsigned short size)
{
    struct __attribute__((packed)) TcpPacketFields
    {
        unsigned short type;
        unsigned short size;
        unsigned short pad;
        unsigned int socket;
    };
    if (m_net)
    {
        CTcpSendBuffer* buf = m_net->Acquire_TcpSendBuffer();
        TcpPacketFields* f = (TcpPacketFields*)buf;
        f->type = type;
        f->size = size;
        f->socket = (unsigned int)m_socket;
        return (char*)f;
    }
    return 0;
}
