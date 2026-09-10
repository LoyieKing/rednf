bool TCPSocket::setOptReuseAdrs(bool b)
{
    int t = 0;
    if (b)
    {
        t = 1;
    }
    else
    {
        t = 0;
    }
    // ORIG: setsockopt → shr $0x1f; return 0 / 1
    if (setsockopt(sock_, 1, 2, &t, 4) < 0)
    {
        return false;
    }
    return true;
