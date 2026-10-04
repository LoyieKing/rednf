int DisPatcher_MoveMap::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_MOVE_MAP& mv = *reinterpret_cast<MSG_MOVE_MAP*>(&msg);
    if (packet.get_byte(mv.m_x) != true)
    {
        return LineFunc(0x1152,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_byte(mv.m_y) != true)
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
    mv.m_1a = packet.get_short(mv.m_trapBits);
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
    if (packet.get_short(*reinterpret_cast<unsigned short*>(mv.m_pad33)) != true)
    {
        return LineFunc(0x116e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (packet.get_int(mv.m_field35) != true)
    {
        return LineFunc(0x1181,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
