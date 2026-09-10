bool UDPSocket::setOptResizeRecvBuf(int size)
{
    if (size < 1)
    {
        return false;
    }
    int iResult = setsockopt(this->sock_, 1, 8, &size, 4);
    if (iResult < 0)
    {
        return false;
    }
    return true;
