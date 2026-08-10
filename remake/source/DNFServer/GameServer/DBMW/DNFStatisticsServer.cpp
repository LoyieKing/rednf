// df_dbmw_r - CStatisticsServer (ORIG DNFStatisticsServer.cpp)
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

// ---- CStatisticsServer（dbmw 同构宏展开）----

CStatisticsServer::CStatisticsServer() : m_type(0xff) { m_port = 0; m_padA = 0; m_flag = 0; m_udpHandler = 0; } 
CStatisticsServer::~CStatisticsServer() {} 
void CStatisticsServer::OnDisconnect() { m_padA = 0; } 
void CStatisticsServer::Init(unsigned char type, std::string& name, unsigned short port, 
               unsigned char flag) 
{ 
    m_type = type; 
    m_name = name; 
    m_port = port; 
    m_udpHandler = new CUdpHandler; 
    m_udpHandler->InitClientSocket(); 
    m_flag = flag; 
} 
void CStatisticsServer::SendToServer(char* buf, int len) 
{ 
    if (m_udpHandler) 
        m_udpHandler->SendToServer(buf, len, m_port, m_name.c_str()); 
} 
char CStatisticsServer::IsValidGameServer() 
{ 
    if (m_type != 0xff) 
        return 1; 
    return 0; 
}

void CStatisticsServer::SendHeartBeat() 
{ 
    if (m_udpHandler) 
    { 
        Packet_Monitor_UDP_HeartBeat pkt; 
        pkt.m_fieldA = 0xc8; 
        SendToServer((char*)&pkt, pkt.packetSize); 
    } 
}

