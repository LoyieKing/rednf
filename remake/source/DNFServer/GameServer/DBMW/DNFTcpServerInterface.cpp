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
    m_socket = (void*)sock;
    m_net = net;
}
char CTcpServer::IsValidServer()
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
void* CTcpServer::GetSocket() { return m_socket; }
void CTcpServer::NotifyHeartbeat() { time(&m_heartbeat); }
void CTcpServer::SetServerType(unsigned char type) { m_index = type; }
unsigned char CTcpServer::GetServerType() { return m_index; }
char* CTcpServer::makePacketHeader(unsigned short type, unsigned short size)
{
    if (!m_net)
        return 0;
    CTcpSendBuffer* buf = m_net->Acquire_TcpSendBuffer();
    char* p = (char*)buf;
    *(unsigned short*)p = type;
    *(unsigned short*)(p + 2) = size;
    *(int*)(p + 6) = (int)m_socket;
    return (char*)buf;
}
