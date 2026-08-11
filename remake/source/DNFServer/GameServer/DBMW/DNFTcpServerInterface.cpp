// df_dbmw_r - DNFTcpServerInterface (ORIG DNFTcpServerInterface.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

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
void* CTcpServer::GetSocket() { return (void*)m_socket; }
void CTcpServer::NotifyHeartbeat() { time(&m_heartbeat); }
void CTcpServer::SetServerType(unsigned char type) { m_index = type; }
unsigned char CTcpServer::GetServerType() { return m_index; }
unsigned short CTcpServer::makePacketHeader(unsigned short type, unsigned short size)
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
        return (unsigned short)(unsigned int)f;
    }
    return 0;
}
