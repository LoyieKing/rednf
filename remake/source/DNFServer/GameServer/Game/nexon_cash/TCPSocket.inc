TCPSocket::TCPSocket()
    : m_socket(INVALID_SOCKET), m_sendRetryCount(0)
{
    memset(&m_peerAdrs, 0, sizeof(m_peerAdrs));   // ORIG memset(this+0x14,0,4)
    m_peerPort = 0;
