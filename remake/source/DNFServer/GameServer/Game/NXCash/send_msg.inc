// ORIG 0x081aec2c（W）：encode(totalLen) → 缓存 length→%esi / data→%ebx
// （0x081aec5c/0x081aec6e 直用寄存器，技巧2），getHandle 结果免槽直填第1参
// （0x081aec79-0x081aec8e，技巧37）；n 槽 -0xc；比较对象是**重调用** length()
// （0x081aeca4-0x081aeca9 cmp -0xc,%eax + setne/test，技巧9 bool 形）；
// 错误分支 strerror 结果 → %ebx 后重 call length 填 printf（0x081aecc4-0x081aecd4）。
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
}