#include "CStatisticServerProxy.h"

#include <cstring>

#include "LogManager.h"
#include "Packet_File_Statistic.h"

// ORIG _ZN21CStatisticServerProxyC1ESsi（0x08471f84，T）：
// string 拷贝构造（this+0）+ m_port = param_2（this+4）+ CUdpHandler 构造（this+8）；
// CUdpHandler 构造可能抛异常，gcc 生成 string 析构清理（ORIG try/catch 同形）。
CStatisticServerProxy::CStatisticServerProxy(std::string ip, int port)
    : m_ip(ip), m_port(port), m_udp()
{
}

// ORIG _ZN21CStatisticServerProxy4InitEv（0x08471fda，T）：
// m_port 为 0 直接成功；否则初始化 UDP 客户端套接字，失败经 LogManager 记日志
// 返回 false（日志函数名参数为 __PRETTY_FUNCTION__ 局部静态，P118）。
bool CStatisticServerProxy::Init()
{
    if (m_port == 0)
        return 1;
    if (m_udp.InitClientSocket() == -1)
    {
        LogManager::logFormat(1, "DF_StatisticServerProxy.cpp",
                              __PRETTY_FUNCTION__, 0x267,
                              "[CStatisticServerProxy::Init] Can't init udp client socket\n");
        return 0;
    }
    return 1;
}

// ORIG _ZN21CStatisticServerProxy10SendPacketEPci（0x08472044，T）：
// m_port 非 0 时经 CUdpHandler 发 UDP 包（端口取 m_port 低 16 位，IP 为 m_ip）。
void CStatisticServerProxy::SendPacket(char* packet, int size)
{
    if (m_port != 0)
        m_udp.SendToServer(packet, size, (unsigned short)m_port, m_ip.c_str());
    return;
}

// ORIG _ZN21CStatisticServerProxy17SendFileStatisticEPKc（0x08472092，T）。
void CStatisticServerProxy::SendFileStatistic(const char* file)
{
    SendFileStatistic(file, "");
}

// ORIG _ZN21CStatisticServerProxy17SendFileStatisticEPKcS1_（0x084720b4，T）：
// Packet_File_Statistic 头 + 双缓冲（文件名 m_fileName / 内容 m_fileData），
// packetSize（+2 ushort）作发送长度。
void CStatisticServerProxy::SendFileStatistic(const char* file, const char* key)
{
    Packet_File_Statistic pkt;
    strcpy(pkt.m_fileName, key);
    strcpy(pkt.m_fileData, file);
    SendPacket((char*)&pkt, pkt.packetSize);
}
