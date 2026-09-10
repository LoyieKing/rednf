int UDPSocket::send(char* data, int length, unsigned short port,
                    const char* host)
{
    if (data == 0 || length <= 0)
        return -1;
    if (host == 0)
        return 0;

    struct sockaddr_in adrs;
    memset(&adrs, 0, sizeof(adrs));
    adrs.sin_family = 2;
    adrs.sin_port = htons(port);
    adrs.sin_addr.s_addr = inet_addr(host);

    int n = static_cast<int>(::sendto(
        m_socket, data, length, 0,
        reinterpret_cast<struct sockaddr*>(&adrs),
        static_cast<socklen_t>(sizeof(adrs))));
    if (n < 0)
    {
        // [b28 BM11] ORIG 立即数序 = 11,11,4（EWOULDBLOCK==EAGAIN 双写 + EINTR 尾，
        // P171 原码链序照抄）；旧形 11,4,11 与 ORIG cmp 立即数错位。
        if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR)
            return 0;
    }
    return n;
