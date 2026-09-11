bool CUdpHandler::SendToClient(char* buf, int len, unsigned short port,
                              const char* ip, unsigned int addr) const
{
    int sent;
    int err;
    if (m_clientSocket == -1)
    {
        cMyTrace trace(__FUNCTION__, 0xe1, 5);
        trace("[%s][%d]", __FUNCTION__, 0xe1);
        return 0;
    }
    if (ip == 0 && addr == 0)
    {
        return 0;
    }
    if (ip != 0)
    {
        addr = inet_addr(ip);
    }
    if (m_serverSocket == -1)
    {
        return 0;
    }
    if (port == 0)
    {
        if (addr == 0)
        {
            sent = send(m_serverSocket, buf, len, 0);
        }
    }
    else
    {
        sockaddr_in to;
        memset(&to, 0, 0x10);
        to.sin_family = AF_INET;
        to.sin_port = htons(port);
        to.sin_addr.s_addr = ntohl(addr);
        sent = sendto(m_serverSocket, buf, len, 0, (sockaddr*)&to, 0x10);
    }
    if (sent == -1)
    {
        err = getErrno();
        switch (err)
        {
        case 0x6f:
        case 0x70:
        case 0x71:
            printf("Error( ECONNREFUSED, EHOSTDOWN, EHOSTUNREACH ) = %d\n", err);
            break;
        case 0x61:
            puts("err EAFNOSUPPORT in send");
            break;
        default:
            printf("err = %d , strerror = %s in send\n", err, strerror(err));
            break;
        }
        return 0;
    }
    if (sent == 0)
    {
        puts("no data sent in send");
        return 0;
    }
    if (len != sent)
    {
        printf("Only %s out of %d bytes sent\n", sent, len);
        return 0;
    }
    else
    {
        return 1;
    }
}