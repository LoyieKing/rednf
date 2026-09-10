int UDPSocket::send(char* buf, const int size, unsigned short nPort, const char* szDestIp)
{
    if ((buf == NULL) || (size < 1))
    {
        return -1;
    }
    else if (szDestIp == NULL)
    {
        return 0;
    }
    sockaddr_in to;
    memset(&to, 0, 0x10);
    to.sin_family = 2;
    to.sin_port = htons(nPort);
    to.sin_addr.s_addr = inet_addr(szDestIp);
    int n_bytes = (int)sendto(this->sock_, buf, size, 0, (sockaddr*)&to, 0x10);
    if (n_bytes < 0)
    {
        if ((errno == EAGAIN) || (errno == EAGAIN) || (errno == EINTR))
        {
            return 0;
        }
    }
    return n_bytes;
