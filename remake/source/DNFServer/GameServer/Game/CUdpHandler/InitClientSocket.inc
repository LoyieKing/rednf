int CUdpHandler::InitClientSocket()
{
    timeval rcvtimeo;
    int err;
    m_clientSocket = socket(AF_INET, SOCK_DGRAM, 0x11);
    if (m_clientSocket == -1)
    {
        err = getErrno();
        printf("Could not create a UDP socket : %d\n", err);
        return -1;
    }
    rcvtimeo.tv_sec = 10;
    rcvtimeo.tv_usec = 0;
    setsockopt(m_clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&rcvtimeo,
               sizeof(rcvtimeo));
    return m_clientSocket;
}