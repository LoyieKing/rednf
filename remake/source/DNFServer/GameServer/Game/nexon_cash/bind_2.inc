bool UDPSocket::bind(unsigned short port, bool nonBlock)
{
    m_port = port;
    memset(m_adrs, 0, sizeof(m_adrs));
    // [b28d-D43] 同上具名视图直存。
    m_sin.m_sinFamily = 2;
    m_sin.m_sinAddr = htonl(0);
    m_sin.m_sinPort = htons(m_port);

    if (::bind(m_socket,
               reinterpret_cast<struct sockaddr*>(m_adrs), 16) != 0)
    {
        int e = errno;
        // [b28d-D43] P251：ORIG 0x081aef15 段为 `mov -0xc(%ebp),%eax; cmp
        // $0x62,%eax; je …; cmp $0x63,%eax; je …; test %eax,%eax; je …`
        // （值载寄存器后多路比较链，非内存直比）⇒ 源码是 switch(e)。
        switch (e)
        {
        case 98: /*EADDRINUSE*/
            printf("Port %d for receiving UDP is in use\n",
                   static_cast<int>(port));
            break;
        case 99: /*EACCES*/
            puts("Cannot assign requested address.");
            break;
        case 0:
            break;
        default:
            printf("Could not bind UDP receive port. Error= %d , strerror=%s\n",
                   e, strerror(e));
            break;
        }
        return false;
    }
    if (nonBlock)
        setOptNonBlock();
    return true;
