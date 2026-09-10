bool UDPSocket::open()
{
    if (m_socket != INVALID_SOCKET)
        return false;
    m_socket = ::socket(2, 2, 0);
    if (m_socket == INVALID_SOCKET)
    {
        int err = errno;
        printf("Could not create a UDP socket : %d\n", err);
        return false;
    }
    return true;
