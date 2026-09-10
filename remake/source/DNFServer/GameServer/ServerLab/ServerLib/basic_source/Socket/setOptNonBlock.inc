int TCPSocket::setOptNonBlock()
{
    int flags = fcntl(sock_, 3, 0);
    flags = flags | 0x800;
    // ORIG: fcntl result via shr $0x1f; return 0 or 1 (not raw ret)
    if (fcntl(sock_, 4, flags) < 0)
    {
        return 0;
    }
    return 1;
