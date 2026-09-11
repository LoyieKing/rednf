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