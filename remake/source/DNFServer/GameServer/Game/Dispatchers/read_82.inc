int Dispatcher_CheckJoinGuild::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081d4b4c：int@0xd（len）→ str@0x11（max 0x17）
    // P1：int@0xd 成员视图折叠单 disp。
    struct __attribute__((packed)) LenView { char m_head[0xd]; int m_len; };
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_int(*reinterpret_cast<int*>(m + 0xd)))
        return LineFunc(0x34db, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(m + 0x11, 0x17, ((LenView*)m)->m_len))
        return LineFunc(0x34de, __PRETTY_FUNCTION__, 0, 0);
    return 0;
