// ORIG 0x081aeb3c：按 body_length 继续收 body 区（flags 同 0x100）。
int NCashImpl::recv_body()
{
    // [b28 BM11] N96/P0 清绑：register asm("esi")/asm("ebx") 存量删除，改实参
    // 内联形（P200/P226 方向）：body_length→body→getHandle 的 R2L 求值序与
    // ORIG 装载序同构；跨 body call 存活的 ushort 零扩展值由 4.4 升格
    // callee-saved。
    int n = static_cast<int>(::recv(
        m_socket.getHandle(), m_inMsg.body(),
        m_inMsg.body_length(), 0x100));
    if (n == 0)
    {
        printf("[%s] connection lost: %s\n", __FUNCTION__, strerror(errno));
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
    return 0;
}