bool TCPSocket::setOptResizeRecvBuf(int size)
{
    if (size < 1)
    {
        return false;
    }
    int optlen;
    int unused = 0;
    (void)unused;
    (void)optlen;
    optlen = 4;
    int ret = setsockopt(sock_, 1, 8, &size, 4);
    if (ret < 0)
    {
        return false;
    }
    return true;
