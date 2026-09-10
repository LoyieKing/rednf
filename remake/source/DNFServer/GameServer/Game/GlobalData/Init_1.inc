bool CmmChannelProxy::Init()
{
    // ORIG 0x0810614a：call 结果 %eax 直比（cmp/sete 物化），无 r 局部。
    if (m_udp.InitClientSocket() == -1)
        return false;
    m_field1c = 0;
    memset(m_buf, 0, 0x80000);
    return true;
