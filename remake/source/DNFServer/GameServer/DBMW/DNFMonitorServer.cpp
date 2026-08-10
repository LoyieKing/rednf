// df_dbmw_r - CMonitorServer (ORIG DNFMonitorServer.cpp)
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

// ---- CMonitorServer（dbmw 同构宏展开）----

CMonitorServer::CMonitorServer() : m_type(0xff) { m_port = 0; m_padA = 0; m_flag = 0; m_udpHandler = 0; } 
CMonitorServer::~CMonitorServer() {} 
void CMonitorServer::OnDisconnect() { m_padA = 0; } 
void CMonitorServer::Init(unsigned char type, std::string& name, unsigned short port, 
               unsigned char flag) 
{ 
    m_type = type; 
    m_name = name; 
    m_port = port; 
    m_udpHandler = new CUdpHandler; 
    m_udpHandler->InitClientSocket(); 
    m_flag = flag; 
} 
void CMonitorServer::SendToServer(char* buf, int len) 
{ 
    if (m_udpHandler) 
        m_udpHandler->SendToServer(buf, len, m_port, m_name.c_str()); 
} 
char CMonitorServer::IsValidGameServer() 
{ 
    if (m_type != 0xff) 
        return 1; 
    return 0; 
}

void CMonitorServer::SendHeartBeat() 
{ 
    if (m_udpHandler) 
    { 
        Packet_Monitor_UDP_HeartBeat pkt; 
        pkt.m_fieldA = 0xc8; 
        SendToServer((char*)&pkt, pkt.packetSize); 
    } 
}

