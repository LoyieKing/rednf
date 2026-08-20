#include "CStatisticServerProxy.h"
#include <cstdio>
#include <cstring>

CStatisticServerProxy::CStatisticServerProxy(std::string ip, int port)
    : BaseServerProxy(const_cast<char*>(ip.c_str()), port, const_cast<char*>("statistic"))
{
}

bool CStatisticServerProxy::Init()
{
    return BaseServerProxy::Init();
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
