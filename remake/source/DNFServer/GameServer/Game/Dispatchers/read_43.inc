int Dispatcher_BossDieCheck::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_BOSS_DIE_CHECK& req = *reinterpret_cast<MSG_BOSS_DIE_CHECK*>(&msg);
    if (!packet.get_short(req.m_field0e))
    {
        return LineFunc(0x2578,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_field10))
    {
        return LineFunc(0x2579,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(req.m_skillCheckSum))
    {
        return LineFunc(0x257b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
