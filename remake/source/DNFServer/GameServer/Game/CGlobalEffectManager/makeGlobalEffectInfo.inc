void CGlobalEffectManager::makeGlobalEffectInfo(PacketGuard& packet) const
{
    packet.put_header( 0, 0xb5);
    packet.put_byte( 1);
    packet.put_int( static_cast<int>(m_activeEffects.size()));
    // [b28c-16] P13：end 缓存具名局部（ORIG it=-0xc、e=-0x10）。
    std::vector<CGlobalEffect>::const_iterator it = m_activeEffects.begin();
    std::vector<CGlobalEffect>::const_iterator e = m_activeEffects.end();
    for (; it != e; ++it)
    {
        it->makeNotifyInfo(packet);
    }
