int DispatcherResetRandomOption::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_RESET_RANDOM_OPTION& req = *reinterpret_cast<MSG_RESET_RANDOM_OPTION*>(&msg);
    if (!packet.get_short(req.m_equipSlot))
    {
        return LineFunc(0x4db2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_scrollSlot))
    {
        return LineFunc(0x4db5,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(req.m_rarityChoice))
    {
        return LineFunc(0x4db8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
