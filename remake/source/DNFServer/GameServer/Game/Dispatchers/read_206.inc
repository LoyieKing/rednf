int Dispatcher_UpgradeItemSeparate::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081cbf2f/0x081cbf74/0x081cbfb9：每次从 0x10(%ebp) 重读 msg（无引用槽，P0）。
    if (!packet.get_short(reinterpret_cast<MSG_ITEM_UPGRADE&>(msg).m_targetSlot))
    {
        return LineFunc(0x228e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG 0x081cbf84 调 get_int(unsigned long&)（msg+0x1d 直接传址）：字段按 ORIG
    // 声明宽度取 Rm 重载（32 位下与 unsigned int 同宽同布局，mangled 为准 P110）。
    if (!packet.get_int(
            reinterpret_cast<unsigned long&>(
                reinterpret_cast<MSG_ITEM_UPGRADE&>(msg).m_targetItemIdx)))
    {
        return LineFunc(0x2290,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(reinterpret_cast<MSG_ITEM_UPGRADE&>(msg).m_materialSlot))
    {
        return LineFunc(0x2292,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG 0x081cbffe/0x081cc005/0x081cc00c：-0xc=0x80（dead）、-0x10=strLen、
    // -0x91..-0x11 的 0x81 字节 dead 缓冲（对齐剥离 rep stos 慢路，P312/P232）。
    int dummyLen = 0x80;                    // ORIG -0xc（dead）
    unsigned int strLen = 0;                // ORIG -0x10
    char deadBuf[0x81] = {0};               // ORIG -0x91..-0x11（dead，复现清零慢路）
    (void)dummyLen;
    (void)deadBuf;
    if (!packet.get_int(strLen))
    {
        return LineFunc(0x229a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(reinterpret_cast<MSG_ITEM_UPGRADE&>(msg).m_28, 0x81,
                        static_cast<int>(strLen)))
    {
        return LineFunc(0x229d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
