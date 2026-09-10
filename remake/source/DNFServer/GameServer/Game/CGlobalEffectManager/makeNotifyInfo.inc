void CGlobalEffect::makeNotifyInfo(PacketGuard& packet) const
{
    packet.put_int( static_cast<int>(m_type));
    packet.put_int(static_cast<int>(m_value * kValueScale));
    // P48/P0：ORIG 无 CSE，strlen(name) 调两次，各自结果寄存器直传入参（无 len 局部槽）。
    packet.put_int( static_cast<int>(strlen(m_name)));
    packet.put_str( m_name, static_cast<int>(strlen(m_name)));
    packet.put_int( static_cast<int>(m_endTime));
