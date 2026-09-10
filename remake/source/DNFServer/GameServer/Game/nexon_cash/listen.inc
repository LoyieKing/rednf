bool TCPSocket::listen(int backlog)
{
    if (::listen(m_socket, backlog) < 0)
    {
        close();
        return false;
    }
    return true;
