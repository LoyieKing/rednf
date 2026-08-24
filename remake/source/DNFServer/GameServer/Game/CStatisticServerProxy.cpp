#include "CStatisticServerProxy.h"
#include <cstdio>
#include <cstring>

CStatisticServerProxy::CStatisticServerProxy(std::string ip, int port)
    : BaseServerProxy(const_cast<char*>(ip.c_str()), port, const_cast<char*>("statistic"))
{
}

CStatisticServerProxy::CStatisticServerProxy()
    : BaseServerProxy(const_cast<char*>(""), 0, const_cast<char*>("statistic"))
{
    // ORIG W 0x082a6b72：CServerProxyMgr 内嵌默认代理的默认构造。
}

// ORIG _ZN21CStatisticServerProxy4InitESsii（0x082a6bc0，W）：
// 记录 IP/端口后转调无参 Init()。ORIG 直写对象头 +0/+4；重建布局中
// IP/端口为 BaseServerProxy::m_ip/m_port —「推断」按语义映射。
void CStatisticServerProxy::Init(std::string ip, int tcpPort, int udpPort)
{
    m_ip = ip;
    m_port = tcpPort;
    Init();
}

// ORIG 0x08471fda：m_tcpHandler.m_sock（this+4）为 0 时直接成功；否则初始化
// 客户端套接字，失败经 LogManager 记日志返回 false。
bool CStatisticServerProxy::Init()
{
    if (m_tcpHandler.m_sock == 0)
        return true;
    if (!BaseServerProxy::Init())
    {
        LogManager::logFormat(1, "DF_StatisticServerProxy.cpp",
                              "bool CStatisticServerProxy::Init()", 0x267,
                              "[CStatisticServerProxy::Init] Can't init udp client socket\n");
        return false;
    }
    return true;
}

void CStatisticServerProxy::SendPacket(char* packet, int size)
{
    if (packet != 0 && size > 0)
        SendTcpPacket(packet, size);
}

void CStatisticServerProxy::SendFileStatistic(const char* file)
{
    SendFileStatistic(file, "");
}

void CStatisticServerProxy::SendFileStatistic(const char* file, const char* key)
{
    if (file == 0) return;
    char packet[0x900];
    int n = std::snprintf(packet, sizeof(packet), "%s%s%s", file,
                          (key && *key) ? " " : "", key ? key : "");
    if (n > 0) SendPacket(packet, n);
}

// CServerProxyMgr<CStatisticServerProxy> 方法模板在 CServerProxyMgr.h 定义，此处强制实例化（原 GameStubs.cpp 提供）。

// CServerProxyMgr<CStatisticServerProxy> 的 4 个方法此前由 GameStubs.cpp 的模板实例化提供，
// 迁移到本 TU 显式实例化（仅实例化 ORIG 需要的方法，避免 createServerProxy/Init 签名冲突）。
#include "CServerProxyMgr.h"
template int CServerProxyMgr<CStatisticServerProxy>::GetStartIndex();
template int CServerProxyMgr<CStatisticServerProxy>::GetEndIndex();
template int CServerProxyMgr<CStatisticServerProxy>::GetNextIndex(int&);
template CStatisticServerProxy* CServerProxyMgr<CStatisticServerProxy>::GetServerProxy(ENUM_SERVER_GROUP);
