// ORIG 0x081c3d46 T _ZN22DisPatcher_GetUserInfo4readER9PacketBufR8MSG_BASE
// （报告 DisPatcher_GetUserInfo/read.md 逐块映射）：get_short→msg+0xd、
// get_byte→msg+0xf；失败各走 LineFunc(0xf5b/0xf5d, PRETTY, 0, 0)。
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
}
