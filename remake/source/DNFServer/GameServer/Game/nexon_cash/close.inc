// ORIG 0x081af80c：
//   if (m_socket == -1) return;
//   close(m_socket); m_socket=-1; memset(this+0x14,0,4); m_peerPort=0;
void TCPSocket::close()
{
    if (m_socket == -1)
        return;
    ::close(m_socket);
    m_socket = -1;
    memset(&m_peerAdrs, 0, 4);   // ORIG 0x081af832-0x081af84b：真实 memset@plt 调用
    m_peerPort = 0;
}