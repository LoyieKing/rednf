void CSchoolMatchEvent::AppendInfo(PacketGuard& packet)
{
    // ORIG 0x082742a8：put_byte(频道数) 后遍历 put_short 各频道号。
    packet.put_byte((int)m_channels.size());
    for (std::vector<int>::iterator it = m_channels.begin();
         it != m_channels.end(); ++it) {
        int value = *it;   // ORIG 经 -0xc 槽中转（Ghidra local_10）
        packet.put_short(value);
    }
}