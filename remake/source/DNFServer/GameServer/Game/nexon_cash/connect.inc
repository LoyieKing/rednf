bool TCPSocket::connect(const char* host, unsigned short port)
{
    struct sockaddr_in adrs;
    memset(&adrs, 0, sizeof(adrs));
    adrs.sin_family = 2;
    adrs.sin_addr.s_addr = inet_addr(host);
    adrs.sin_port = htons(port);

    // [b28d-solo-nexon_cash] P218/P290：len 声明为 int（非 socklen_t/unsigned）
    // 触发 4.4 提前装载 `mov -0xc(%ebp),%ecx`（ORIG 0x081afa3d 序），DIFF→AE。
    int len = sizeof(adrs);
    if (::connect(m_socket, reinterpret_cast<struct sockaddr*>(&adrs), len) < 0)
        return false;

    memcpy(&m_peerAdrs, &adrs.sin_addr, 4);
    m_peerPort = adrs.sin_port;
    return true;
