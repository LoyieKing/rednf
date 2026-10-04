int DisPatcher_DieCharacInDungeon::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_DIE_CHARACTER& req = *reinterpret_cast<MSG_DIE_CHARACTER*>(&msg);
    {
        unsigned short v0 = 0;
        if (!packet.get_short(v0))
        {
            return LineFunc(0xca9, __PRETTY_FUNCTION__, 0, 0);
        }
        req.m_0d = v0;
    }
    {
        unsigned short v1 = 0;
        if (!packet.get_short(v1))
        {
            return LineFunc(0xcaa, __PRETTY_FUNCTION__, 0, 0);
        }
        req.m_0f = v1;
    }
    return 0;
}
