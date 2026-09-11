// ORIG 0x081aea46（W）：MSG_WAITALL(0x100) 预读头并解码，随后转 recv_body。
// buf → %ebx（0x081aea58 call data 后直落寄存器，技巧2），getHandle 结果免槽
// 直填第1参（0x081aea66-0x081aea7b），n 槽 -0xc，frame 0x24 + pop ebx。
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
}