int NCashImpl::send_msg(unsigned int totalLen)
{
    m_outMsg.encode_header(totalLen);
    // [b28d-D43] P0/N96 清绑：register asm("esi")/asm("ebx") 删除，改实参内联形。
    int n = static_cast<int>(::send(m_socket.getHandle(), m_outMsg.data(),
                                    m_outMsg.length(), 0x100));
    if (n != static_cast<int>(m_outMsg.length()))   // 0x081aeca9 cmp -0xc,%eax
    {
        // [b28d-D43] P0/N96 清绑：err 改内联实参。
        printf("send error: send bytes: %d, message length: %d, error: %s\n",
               n, static_cast<int>(m_outMsg.length()), strerror(errno));
        m_connected = false;
        m_socket.close();
        return 2;
    }
    return 0;
