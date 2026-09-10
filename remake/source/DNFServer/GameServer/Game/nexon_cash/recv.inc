int TCPSocket::recv(char* buffer, unsigned int length)
{
    if (buffer == 0 || length == 0)
        return -1;

    int n = static_cast<int>(::read(m_socket, buffer, length));
    if (n < 0)
    {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN || errno == 0)
            return 0;   // 0x081af7da 共岛 mov $0
    }
    else
    {
        if (n == 0)
            return -1;   // ORIG 0x081af7e7 n>=0 岛（jns +0x73）
    }
    return n;            // ORIG 0x081af7ee 三路共享尾块（6a/77 jne 同标）
