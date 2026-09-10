bool TCPSocket::accept(TCPSocket& peer)
{
    socklen_t len = 16;  // sizeof(sockaddr_in)
    peer.m_socket = ::accept(m_socket, reinterpret_cast<struct sockaddr*>(
                                            &peer.m_sinFamily), &len);
    if (peer.m_socket < 0 || peer.m_socket == INVALID_SOCKET)
        return false;

    // ORIG 0x081aff78：memcpy(dst=&peer.m_peerAdrs(+0x14), src=peer+0x08 单步)。
    memcpy(&peer.m_peerAdrs, &peer.m_sinAddr, 4);
    // ORIG 0x081aff87：movzwl 0x6(%eax) 单步 disp 直读 sin_port（peer+0x06），
    // 0x081aff91 mov %dx,0x18(%eax) 存 m_peerPort。
    peer.m_peerPort = peer.m_sinPort;
    peer.setOptNonBlock();
    return true;
