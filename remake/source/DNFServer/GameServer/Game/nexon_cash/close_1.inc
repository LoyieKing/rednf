void UDPSocket::close()
{
    if (m_socket == -1)
        return;
    ::close(m_socket);
    m_socket = INVALID_SOCKET;
    m_port = 0;
