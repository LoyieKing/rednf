bool UDPSocket::bind(const char* host, unsigned short port)
{
    m_port = port;
    memset(m_adrs, 0, sizeof(m_adrs));
    // [b28d-D43] 具名视图直存（ORIG 0x081aedfc/0x081aee10/0x081aee2b 三条
    // this+disp 单条写；旧 memcpy+局部 addr/netPort 多 11 条）。htons 实参按
    // ORIG 读 m_port（0x081aee19 双段 movzwl），非形参 port。
    m_sin.m_sinFamily = 2;                  // AF_INET
    m_sin.m_sinAddr = inet_addr(host);
    m_sin.m_sinPort = htons(m_port);

    if (::bind(m_socket,
               reinterpret_cast<struct sockaddr*>(m_adrs), 16) != 0)
    {
        printf("Udp Port binding fail, %s , %d\n", host,
               static_cast<int>(port));
        return false;
    }
    return true;
