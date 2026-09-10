int DisPatcher_SelectDungeon::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SELECT_DUNGEON& req = *reinterpret_cast<MSG_SELECT_DUNGEON*>(&msg);
    if (!packet.get_short(req.m_dungeonIdx))
    {
        return LineFunc(0x18da,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_dungeonType)))
    {
        return LineFunc(0x18db,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_field10)))
    {
        return LineFunc(0x18e2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_field11)))
    {
        return LineFunc(0x18e4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
// [b28d-29] N82/N93：槽号平移与块序（ORIG 无 -0x10 副本，4.4 多 mov 0xc(%eax) 重读），语义集合全等  (ORIG 0x081c8e9c insn 204->199)
