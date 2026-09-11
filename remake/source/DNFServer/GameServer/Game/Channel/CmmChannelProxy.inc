CmmChannelProxy::CmmChannelProxy(std::string ip, int port)
    : m_ip(ip), m_port(port)
{
    // ORIG 0x081060f4：string 拷贝 + port + CUdpHandler@+0x10 构造（成员自动）。
}