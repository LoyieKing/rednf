// CPCRoomServerProxy 仅在本 TU 定义（无独立源文件）。ORIG T 0x084717dc：
// CNetwork 基类构造 + string m_ip(ip)（初始化列表）；函数体写 m_tcpPort=port
// 再清 m_socketState（汇编：string 构造后 mov port→0x30dfc、movl $0→0x30e00）。
CPCRoomServerProxy::CPCRoomServerProxy(char* ip, int port)
    : m_ip(ip)
{
    m_tcpPort = port;
    m_socketState = ENUM_SOCKET_STATE_DISCONNECTED;
}