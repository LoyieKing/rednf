bool TCPSocket::setOptResizeSendBuf(int size)
{
    if (size <= 0)
        return false;
    int dead_vals[2];
    dead_vals[1] = 0;   // ORIG 0x081affe4
    dead_vals[0] = 4;   // ORIG 0x081affeb
    int ret = ::setsockopt(m_socket, 1, 7 /*SO_SNDBUF*/, &size, sizeof(size));
    if (ret < 0)
        return false;
    return true;
