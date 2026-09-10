bool UDPSocket::pollReadEvent() const
{
    fd_set fds;
    struct timeval timeout;
    int ret;
    unsigned int i;
    fd_set* pset;
    pset = &fds;
    for (i = 0; i <= 0x1f; ++i)
        pset->fds_bits[i] = 0;
    FD_SET(static_cast<unsigned int>(m_socket), &fds);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0x1388;
    ret = ::select(2, &fds, 0, 0, &timeout);
    if (ret == 0)
        return false;
    return true;
