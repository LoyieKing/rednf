int Dispatcher_SoloTeleport::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SOLO_TELEPORT& req = reinterpret_cast<MSG_SOLO_TELEPORT&>(msg);
    // ORIG 0x081e1b9c：get_int 直收 `mov -0xc,%eax; add $0x30,%eax` 现址（无 temp+回写）。
    if (!packet.get_int(reinterpret_cast<int&>(req.m_30)))
    {
        return LineFunc(0x5778,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    if (!packet.get_int(req.m_34))
    {
        return LineFunc(0x577b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG 0x081e1b9c：`cmp $-1; je END`（==-1 直落尾 return 0，早退岛无物化）——
    // 改写为正向包裹，坐标读块 fall-through。
    if (req.m_34 != -1)
    {
        // +0x39..+0x40 即 stPartyTelePort 字段(MSG_SOLO_TELEPORT 布局)
        if (!packet.get_byte(req.m_tp.m_village))   // +0x39 unsigned char&
        {
            return LineFunc(0x577f,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(req.m_tp.m_areaIndex)) // +0x3a unsigned char&
        {
            return LineFunc(0x5780,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_short(req.m_tp.m_posx))     // +0x3c short&
        {
            return LineFunc(0x5781,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_short(req.m_tp.m_posy))     // +0x3e short&
        {
            return LineFunc(0x5782,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(req.m_tp.m_direction)) // +0x40 unsigned char&
        {
            return LineFunc(0x5783,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    return 0;
