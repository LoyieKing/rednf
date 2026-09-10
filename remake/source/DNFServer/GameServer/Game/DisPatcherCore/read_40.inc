int DisPatcher_DieCharacInDungeon::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_DIE_CHARACTER& req = *reinterpret_cast<MSG_DIE_CHARACTER*>(&msg);
    if (!packet.get_short(
            *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&req) + 0xd)))
    {
        return LineFunc(0xca9,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(
            *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&req) + 0xf)))
    {
        return LineFunc(0xcaa,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
