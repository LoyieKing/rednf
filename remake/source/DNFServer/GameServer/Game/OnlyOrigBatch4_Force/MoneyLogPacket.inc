// ORIG W 0x08694602（C2 同址）：PacketHeader(0x2723, 0x1e) + 5 int 清零。
MoneyLogPacket::MoneyLogPacket()
    : PacketHeader(0x2723, 0x1e)
{
    m_characNo = 0;
    m_money0 = 0;
    m_money1 = 0;
    m_accId = 0;
    m_money2 = 0;
}