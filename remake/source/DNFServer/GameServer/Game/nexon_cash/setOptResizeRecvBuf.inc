bool TCPSocket::setOptResizeRecvBuf(int size)
{
    if (size <= 0)
        return false;
    int dead_vals[2];
    dead_vals[1] = 0;
    dead_vals[0] = 4;
    int ret = ::setsockopt(m_socket, 1, 8 /*SO_RCVBUF*/, &size, sizeof(size));
    if (ret < 0)
        return false;
    return true;
