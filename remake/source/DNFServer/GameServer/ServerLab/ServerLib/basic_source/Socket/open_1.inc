bool UDPSocket::open()
{
    // ORIG: if sock_ != -1 return false first; else socket(2,2,0)
    if (sock_ != -1)
    {
        return false;
    }
    sock_ = socket(2, 2, 0);
    if (sock_ == -1)
    {
        int err = errno;
        printf("Could not create a UDP socket : %d\n", err);
        return false;
    }
    return true;
