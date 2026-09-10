bool TCPSocket::setOptLinger(bool b)
{
    linger l;
    // ORIG: if/else → eax → store l_onoff; l_linger = 0
    // Use ternary to keep 0/1 in eax at join then store
    l.l_onoff = b ? 1 : 0;
    l.l_linger = 0;
    if (setsockopt(sock_, 1, 0xd, &l, 8) < 0)
    {
        return false;
    }
    return true;
