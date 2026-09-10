int DisPatcher_MoveMap::read(PacketBuf& packet, MSG_BASE& msg)
{
    MoveMapMsgView& mv = *reinterpret_cast<MoveMapMsgView*>(&msg);
    if (packet.get_byte(*reinterpret_cast<char*>(&mv.m_x)) != true)
    {
        return LineFunc(0x1152,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_byte(*reinterpret_cast<char*>(&mv.m_y)) != true)
    {
        return LineFunc(0x1155,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_int(mv.m_fieldF) != true)
    {
        return LineFunc(0x1158,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_int(mv.m_field13) != true)
    {
        return LineFunc(0x1159,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_byte(mv.m_field17) != true)
    {
        return LineFunc(0x115d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    mv.m_1a = packet.get_short(
        *reinterpret_cast<unsigned short*>(mv.m_pad18));   // ORIG `mov %al,0x1a(%edx)`
    for (int local_14 = 0; local_14 < 4; local_14 = local_14 + 1)
    {
        if (packet.get_short(mv.m_1b[local_14]) != true)
        {
            return LineFunc(0x1164,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    for (int local_10 = 0; local_10 < 4; local_10 = local_10 + 1)
    {
        if (packet.get_int(mv.m_23[local_10]) != true)
        {
            return LineFunc(0x1169,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    if (packet.get_short(mv.m_33) != true)
    {
        return LineFunc(0x116e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_int(mv.m_35) != true)                  // ORIG add $0x35 单步
    {
        return LineFunc(0x1181,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
