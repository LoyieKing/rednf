int DisPatcher_GetUserInfo::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081...：无指针局部，实参内联 `mov 0x10(%ebp),%eax; add $0xd`（帧 0x18）。
    if (!packet.get_short(reinterpret_cast<MSG_USERINFO_REQ*>(&msg)->m_targetId))   // +0x0d
    {
        return LineFunc(0xf5b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<MSG_USERINFO_REQ*>(&msg)->m_slot))        // +0x0f
    {
        return LineFunc(0xf5d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
