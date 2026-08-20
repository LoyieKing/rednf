// df_game_r 还原 —— CSchoolServerProxy（学校服务器 UDP 代理）。
// 函数与布局参照 docs/class_func_reports/CSchoolServerProxy.md。

#include "CSchoolServerProxy.h"

#include "LogManager.h"

CSchoolServerProxy::CSchoolServerProxy(std::string ip, int port)
    : m_ip(ip), m_port(port), m_udpHandler()
{
}

bool CSchoolServerProxy::Init()
{
    const int result = m_udpHandler.InitClientSocket();
    if (result == -1) {
        LogManager::logFormat(1, "DF_SchoolServerProxy.cpp",
                              "bool CSchoolServerProxy::Init()", 0x3c,
                              "[CSchoolServerProxy::Init] Can't init udp client socket\n");
        return false;
    }
    return true;
}

void CSchoolServerProxy::SendPacket(char* packet, int length)
{
    m_udpHandler.SendToServer(packet, length,
                               static_cast<unsigned short>(m_port),
                               m_ip.c_str());
}
