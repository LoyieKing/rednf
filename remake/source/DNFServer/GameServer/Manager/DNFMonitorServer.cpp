// df_manager_r — CMonitorServer（ORIG DNFMonitorServer.cpp）
#include "DNFMonitorServer.h"

#include "DNFUdpHandler.h"

CMonitorServer::CMonitorServer() : m_index(0xff), m_name()
{
    m_port = 0;
    m_connected = 0;
    m_heartBeat = 0x14;
    m_fieldC = 0;
    m_udpHandler = 0;
}

CMonitorServer::~CMonitorServer() {}

bool CMonitorServer::IsValidMonitorServer()
{
    if (m_index == 0xff)
        return 0;
    return 1;
}

void CMonitorServer::SendToServer(char* buf, int len)
{
    if (m_udpHandler)
        m_udpHandler->SendToServer(buf, len, m_port, m_name.c_str());
}

void CMonitorServer::Init(std::string& name, unsigned short port, unsigned char flag)
{
    m_name = name;
    m_port = port;
    m_udpHandler = new CUdpHandler;
    m_udpHandler->InitClientSocket();
    m_index = flag;
}

void CMonitorServer::ResetHeartBeat() { m_heartBeat = 0x14; m_fieldC = 0; }
bool CMonitorServer::IsConnected() { return m_connected; }
void CMonitorServer::SetConnFlag(bool flag) { m_connected = flag; }
void CMonitorServer::OnDisconnect() { m_connected = 0; ResetHeartBeat(); }
bool CMonitorServer::IsHeartBeatTimeOver()
{
    if (--m_heartBeat == 0)
    {
        if (++m_fieldC > 0x14)
            return 1;
        m_heartBeat = 0x14;
    }
    return 0;
}
