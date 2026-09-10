bool UDPSocket::bind(const char* strIp, unsigned short port)
{
    this->port_ = port;
    memset(&this->adrs_, 0, 0x10);
    this->adrs_.sin_family = 2;
    this->adrs_.sin_addr.s_addr = inet_addr(strIp);
    this->adrs_.sin_port = htons(this->port_);
    // ORIG: test/setne on bind eax; fail → printf + return 0; else return 1
    if (::bind(this->sock_, (sockaddr*)&this->adrs_, 0x10) != 0)
    {
        printf("Udp Port binding fail, %s , %d\n", strIp, (unsigned int)port);
        return false;
    }
    return true;
