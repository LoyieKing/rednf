Packet_Monitor_UDP_Reply_UserInfo::Packet_Monitor_UDP_Reply_UserInfo()
    : PacketHeader(0x3ea, 0x16e6), m_count(0), m_channel(255)
{
    // ORIG（0x3ea/0x16e6）：从 m_items(+0xc) 起零填充整个报文体（0x16da 字节，
    // 即 0x16e6 报文长 - 0xc 头）= 75 * 0x4e，与 m_items 尺寸一致。
    memset(m_items, 0, 0x16da);
