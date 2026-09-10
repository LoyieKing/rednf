void TCPSocket::close()
{
    if (sock_ != -1)
    {
        ::close(sock_);
        sock_ = -1;
        memset(c_adrs_, 0, 4);
        port_ = 0;
    }
    else
    {
        __asm__ __volatile__("nop");
    }
