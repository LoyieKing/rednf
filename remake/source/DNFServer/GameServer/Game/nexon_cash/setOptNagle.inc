bool TCPSocket::setOptNagle(bool on)
{
    int flag = on ? 1 : 0;
    int ret = ::setsockopt(m_socket, 6, 1, &flag, sizeof(flag));
    return ret == 0;
