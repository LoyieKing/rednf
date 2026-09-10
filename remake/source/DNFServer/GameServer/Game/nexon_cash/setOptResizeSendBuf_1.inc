bool UDPSocket::setOptResizeSendBuf(int size)
{
    if (size <= 0)
        return false;
    int ret = ::setsockopt(m_socket, 1, 7, &size, sizeof(size));
    if (ret < 0)
        return false;
    return true;
