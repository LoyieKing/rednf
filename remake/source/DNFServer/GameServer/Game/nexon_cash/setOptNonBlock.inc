bool TCPSocket::setOptNonBlock()
{
    int flags = ::fcntl(m_socket, 3 /*F_GETFL*/, 0);
    flags |= 0x800;
    if (::fcntl(m_socket, 4 /*F_SETFL*/, flags) < 0)
        return false;
    return true;
