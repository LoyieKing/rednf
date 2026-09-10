int UDPSocket::recv(char* buf, const int size)
{
    if ((buf == NULL) || (size < 1))
    {
        return -1;
    }
    int fromLen = 0x10;
    int len = (int)recvfrom(this->sock_, buf, size, 0, (sockaddr*)&this->from_, (socklen_t*)&fromLen);
    if (len < 0)
    {
        if ((errno == EAGAIN) || (errno == EINTR) || (errno == EAGAIN))
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    return len;
