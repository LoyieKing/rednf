bool UDPSocket::setOptNonBlock()
{
    int flags = fcntl(this->sock_, 3, 0);
    flags = flags | 0x800;
    if (fcntl(this->sock_, 4, flags) < 0)
    {
        return false;
    }
    return true;
