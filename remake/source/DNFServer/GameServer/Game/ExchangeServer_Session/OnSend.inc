int CSession::OnSend()
{
    // ORIG 0x8490364：call send_packet 后 shr $0x1f,%eax 直测符号位，无局部槽
    // （P0 槽-局部律：寄存器直传 ⇒ 内联右值）。
    if (CNetwork<95000, 450000>::send_packet() < 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1e4, 0)("OnSend() Socket[%d] Error(%s)",
            GetSocket(), strerror(errno));
        return -1;
    }
    // ORIG 尾部读 this+0x18(m_recvOffset) 作为返回值（unsigned int → int）。
    return m_recvOffset;
