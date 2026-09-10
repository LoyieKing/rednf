void UDPSocket::close()
{
    if (sock_ != -1)
    {
        ::close(sock_);
        sock_ = -1;
        port_ = 0;
    }
    else
    {
        __asm__ __volatile__("nop");
    }
