bool TCPSocket::connect(const char* ip, unsigned short port)
{
    sockaddr_in svradr;
    memset(&svradr, 0, 0x10);
    svradr.sin_family = 2;
    svradr.sin_addr.s_addr = inet_addr(ip);
    svradr.sin_port = htons(port);
    int len = 0x10;
    // ORIG: connect result via shr $0x1f; fail path first (errno/strerror/log)
    if (::connect(sock_, (sockaddr*)&svradr, len) < 0)
    {
        G_TraceLog()->sysLog(7, "CONNECTION FAIL IP=%s, PORT=%d, reason=%s",
                             ip, (unsigned int)port, strerror(errno));
        return false;
    }
    memcpy(c_adrs_, &svradr.sin_addr, 4);
    port_ = svradr.sin_port;
    return true;
