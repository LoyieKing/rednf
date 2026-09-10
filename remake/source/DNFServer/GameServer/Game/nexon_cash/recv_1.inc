int UDPSocket::recv(char* buffer, int length)
{
    if (buffer == 0 || length <= 0)
        return -1;

    // [b28d-solo-nexon_cash] P218/P290：addrlen 由 socklen_t 改 int + 显式
    // reinterpret_cast<socklen_t*> 触发 4.4 提前 `lea -0x10(%ebp),%ebx`
    // （ORIG 0x081af0ff 序），P75 装载时刻差消除，DIFF→AE。
    int addrlen = 16;
    int n = static_cast<int>(::recvfrom(
        m_socket, buffer, length, 0,
        reinterpret_cast<struct sockaddr*>(m_from),
        reinterpret_cast<socklen_t*>(&addrlen)));
    if (n < 0)
    {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN)
            return 0;
        else
            return -1;
    }
    return n;
