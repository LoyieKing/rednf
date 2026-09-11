// ORIG _ZN21CStatisticServerProxyC1ESsi（0x08471f84，T）：
// string 拷贝构造（this+0）+ m_port = param_2（this+4）+ CUdpHandler 构造（this+8）；
// CUdpHandler 构造可能抛异常，gcc 生成 string 析构清理（ORIG try/catch 同形）。
CStatisticServerProxy::CStatisticServerProxy(std::string ip, int port)
    : m_ip(ip), m_port(port), m_udp()
{
}