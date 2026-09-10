int Dispatcher_Mercenary_Return::read(PacketBuf& packet, MSG_BASE& msg)   // ORIG 0x081dd904 (39 insn)
{
    // msg+0xd byte / msg+0x10 int；失败走 LineFunc(0x4e88/0x4e8a, pretty 名)。
    // ORIG 0x081dd904：arg2 直 `movl $<A>,0x4(%esp)`（rodata 地址立即数，非 static 指针二级）。
    // ORIG 0x81dd90a：顶部 char* m→-0xc 槽，两处地址均经槽 `mov -0xc`+add。
    struct MercView          // 自然对齐即达 m_field10@+0x10（0xd+1+2=16 已 4 对齐）
    {
        char m_head[0xd];
        char m_field0d;          // +0x0d byte
        char m_pad0e[2];         // +0x0e..+0x0f
        unsigned int m_field10;  // +0x10
    };
    MercView& m = reinterpret_cast<MercView&>(msg);
    if (!packet.get_byte(m.m_field0d))
        return LineFunc(0x4e88, __PRETTY_FUNCTION__, 0, 0);
    // ORIG 0x81dd9?：`add $0x10,%eax` 单步地址（同一 m 槽现址）。
    if (!packet.get_int(m.m_field10))
        return LineFunc(0x4e8a, __PRETTY_FUNCTION__, 0, 0);
    return 0;
