bool TCPSocket::setOptReuseAdrs(bool on)
{
    int flag = 0;
    if (on)
        flag = 1;
    else
        flag = 0;
    if (::setsockopt(m_socket, 1, 2, &flag, sizeof(flag)) < 0)
        return false;
    return true;
