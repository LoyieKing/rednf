int DisPatcher_RepairEquip::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_REPAIR_EQUIP& req = *reinterpret_cast<MSG_REPAIR_EQUIP*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_0d)))
    {
        return LineFunc(0x14e6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(&req.m_0e)))
    {
        return LineFunc(0x14e7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_10))
    {
        return LineFunc(0x14e9,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
