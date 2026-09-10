int DisPatcher_UseSkill::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_USE_SKILL& req = *reinterpret_cast<MSG_USE_SKILL*>(&msg);
    // 具名字段（MSG_USE_SKILL pack(1)；ORIG 0x081cac6d add $0xd / ERc → 1B；
    //  0x081cad39 add $0x11 / ERt → 2B，落 m_padTail[2] 内，同宽度引用视图）
    if (!packet.get_byte(req.m_gapD))
    {
        return LineFunc(0x1fce,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(req.m_slot))
    {
        return LineFunc(0x1fd0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_targetId))
    {
        return LineFunc(0x1fd2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(
            *reinterpret_cast<unsigned short*>(req.m_padTail)))
    {
        return LineFunc(0x1fd9,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
