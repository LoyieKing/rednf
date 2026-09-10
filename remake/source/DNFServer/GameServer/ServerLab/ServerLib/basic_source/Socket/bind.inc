bool TCPSocket::bind(unsigned short port, bool bNonBlock)
{
    setOptReuseAdrs(true);
    sockaddr_in svradr;
    memset(&svradr, 0, 0x10);
    svradr.sin_family = 2;
    svradr.sin_port = htons(port);
    svradr.sin_addr.s_addr = 0;
    // ORIG: bind result via shr $0x1f (ret < 0) without storing
    if (::bind(sock_, (sockaddr*)&svradr, 0x10) < 0)
    {
        close();
        return false;
    }
    if (bNonBlock)
    {
        setOptNonBlock();
    }
    printf("succeeded in binding TCP socket port #%d\n", (unsigned int)port);
    return true;
