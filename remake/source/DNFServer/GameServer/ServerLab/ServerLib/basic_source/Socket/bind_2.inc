bool UDPSocket::bind(unsigned short port, bool bNonBlock)
{
    this->port_ = port;
    memset(&this->adrs_, 0, 0x10);
    this->adrs_.sin_family = 2;
    this->adrs_.sin_addr.s_addr = htonl(0);
    this->adrs_.sin_port = htons(this->port_);
    // ORIG: test/setne on bind eax; fail path first, then success
    if (::bind(this->sock_, (sockaddr*)&this->adrs_, 0x10) != 0)
    {
        int err = errno;
        switch (err)
        {
        case 0x62:
            printf("Port %d for receiving UDP is in use\n", (unsigned int)port);
            break;
        case 0x63:
            puts("Cannot assign requested address");
            break;
        case 0:
            break;
        default:
            printf("Could not bind UDP receive port. Error= %d , strerror = %s\n",
                   err, strerror(err));
            break;
        }
        return false;
    }
    if (bNonBlock)
    {
        setOptNonBlock();
    }
    return true;
