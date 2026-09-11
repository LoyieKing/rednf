// ==== Dispatcher_Skill_Command_Customizing::read (ORIG 0x081d29ac) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d29ac orig/ours=103/101 ndiff-blocks=16
//   帧差 sub $28 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x28,%esp | movl   $&_ZZN36Dispatcher_Skill_Command_Customizing4readER9PacketBufR8MSG_BASEE19__PRETTY_FUNCTION__,0x4(%esp) | mov    0x10(%ebp)
int Dispatcher_Skill_Command_Customizing::read(PacketBuf& packet, MSG_BASE& msg)
    {
    // [G6·read_329 负结论 2026-09-11] 试「去 m 槽 + commandCount/keyCount 改 int」
    //   （ORIG 0x081d29b3 `movzbl 0xd(%eax); movsbl %al,%eax; mov %eax,-0x18`）⇒ 实测
    //   101→**105**（劣化 4）⇒ 回滚保 `char* m` + char 计数形。
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(m[0xd]))
    {
        return LineFunc(0x2ff8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    const char commandCount = m[0xd];
    for (int i = 0; i < (int)commandCount; ++i)
    {
        if (!packet.get_short(*reinterpret_cast<short*>(m + i * 8 + 0xe)))
        {
            return LineFunc(0x2fff,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(m[i * 8 + 0x10]))
        {
            return LineFunc(0x3002,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        const char keyCount = (char)m[i * 8 + 0x10];
        for (int j = 0; j < (int)keyCount; ++j)
        {
            if (!packet.get_byte(m[i * 8 + j + 0x11]))
            {
                return LineFunc(0x3009,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
        }
    }
    return 0;
}