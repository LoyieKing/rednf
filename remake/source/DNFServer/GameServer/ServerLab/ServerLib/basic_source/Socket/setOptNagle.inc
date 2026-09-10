bool TCPSocket::setOptNagle(bool b)
{
    int opt_val = b ? 1 : 0;
    int ret = setsockopt(sock_, 6, 1, &opt_val, 4);
    return ret == 0;
