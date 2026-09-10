void TCPSocket::shutdown(int how)
{
    // 忠实转写：ORIG 本函数未发起 shutdown 系统调用（内联裁剪后的残体）。
    if (m_socket == INVALID_SOCKET)
        return;
