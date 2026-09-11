// ORIG 0x081ae5ce（W）：m_bodyLength=ntohs(*(u16*)netLen)（0x081ae5d7 add $5
// 单步后 movzwl 直读，无 copy 槽）；u16 无符号 cmp $0x200 超 → 清零返 false。
inline bool NCashMessage::decode_header()
{
    m_bodyLength = ntohs(*reinterpret_cast<unsigned short*>(m_netLenB));
    if (m_bodyLength > 0x200)
    {
        m_bodyLength = 0;
        return false;
    }
    return true;
}