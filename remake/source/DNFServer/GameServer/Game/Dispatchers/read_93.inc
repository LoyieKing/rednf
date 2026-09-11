int Dispatcher_UserHistoryLog::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081da5d2：byte@0xd → int@0x10（len）→ str@0x14（max 0x100）
    // P1：int@0x10 成员视图折叠。
    MSG_USER_HISTORY_LOG& m = reinterpret_cast<MSG_USER_HISTORY_LOG&>(msg);
    if (!packet.get_byte(m.m_0d))
        return LineFunc(0x48a9, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(m.m_10))
        return LineFunc(0x48ac, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(m.m_14, 0x100, m.m_10))
        return LineFunc(0x48af, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}