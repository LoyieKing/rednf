// ==== Dispatcher_Skill_Command_Customizing::read (ORIG 0x081d29ac) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d29ac orig/ours=103/101 ndiff-blocks=16
//   帧差 sub $28 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x28,%esp | movl   $&_ZZN36Dispatcher_Skill_Command_Customizing4readER9PacketBufR8MSG_BASEE19__PRETTY_FUNCTION__,0x4(%esp) | mov    0x10(%ebp)
int Dispatcher_Skill_Command_Customizing::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SKILL_COMMAND& req = *reinterpret_cast<MSG_SKILL_COMMAND*>(&msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_commandCount)))
    {
        return LineFunc(0x2ff8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    const char commandCount = static_cast<char>(req.m_commandCount);
    for (int i = 0; i < (int)commandCount; ++i)
    {
        if (!packet.get_short(reinterpret_cast<short&>(req.m_entries[i].m_command)))
        {
            return LineFunc(0x2fff,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(reinterpret_cast<char&>(req.m_entries[i].m_count)))
        {
            return LineFunc(0x3002,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        const char keyCount = static_cast<char>(req.m_entries[i].m_count);
        for (int j = 0; j < (int)keyCount; ++j)
        {
            if (!packet.get_byte(reinterpret_cast<char&>(req.m_entries[i].m_keys[j])))
            {
                return LineFunc(0x3009,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
        }
    }
    return 0;
}
