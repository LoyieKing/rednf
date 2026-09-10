bool TCPSocket::bind(unsigned short port, bool nonBlock)
{
    setOptReuseAdrs(true);

    struct sockaddr_in adrs;
    memset(&adrs, 0, sizeof(adrs));
    adrs.sin_family = 2;
    adrs.sin_port = htons(port);
    adrs.sin_addr.s_addr = 0;

    if (::bind(m_socket, reinterpret_cast<struct sockaddr*>(&adrs),
               sizeof(adrs)) < 0)
    {
        close();
        return false;
    }
    if (nonBlock)
        setOptNonBlock();
    printf("TCP Port binding success, port = %d\n", static_cast<int>(port));
    return true;
