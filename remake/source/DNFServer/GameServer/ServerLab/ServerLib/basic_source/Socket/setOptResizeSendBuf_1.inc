bool UDPSocket::setOptResizeSendBuf(int size)
{
    if (size < 1)
    {
        return false;
    }
    int iResult = setsockopt(this->sock_, 1, 7, &size, 4);
    if (iResult < 0)
    {
        return false;
    }
    return true;
