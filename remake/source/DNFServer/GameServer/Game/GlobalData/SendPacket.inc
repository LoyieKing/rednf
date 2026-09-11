// ORIG SendPacket 0x0810637c：write 发送；EAGAIN/EINTR/0 → 0（可重试），
// 其它错误 cMyTrace 后返回 -1。
int CmmChannelProxy::SendPacket(char* buf, int len)
{
    // ORIG 0x0810638b：fd 每处成员直读（this+0x18），无局部 spill。
    // ORIG 0x081063a4：赋值即条件 + (bool) 物化 → cmp/setle/test/je。
    int n;
    if ((bool)((n = write(m_sock, buf, len)) <= 0))
    {
        if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK ||
            errno == 0)
        {
            return 0;
        }
        // ORIG 0x081063e2：临时对象表达式 —— 实参先评估（strerror/errno/m_sock
        // 落 %ebx/%edi/%esi），随后 ctor，再 operator()。
        cMyTrace(__PRETTY_FUNCTION__, 200, 0)(
            "SEND ERROR DISCONNNECT NOW FD[%d] : %d(%s)",
            m_sock, errno, strerror(errno));
        return -1;
    }
    return n;
}