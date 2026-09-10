int NCashImpl::recv_msg()
{
    // [b28d-D43] P0/N96 清绑：register asm("ebx") 删除，改实参内联形。
    int n = static_cast<int>(::recv(m_socket.getHandle(), m_inMsg.data(), 3, 0x100));
    if (n == 0)
    {
        printf("[%s] connection lost: %s\n", __FUNCTION__,
               strerror(errno));
        m_connected = false;
        m_socket.close();
        return 2;
    }
    if (n < 0)
    {
        printf("[%s] recv error: %s\n", __FUNCTION__, strerror(errno));
        m_connected = false;
        m_socket.close();
        return 2;
    }

    m_inMsg.decode_header();
    return recv_body();
