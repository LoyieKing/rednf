int Dispatcher_ChangeLetterStat::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081ce152：具名指针槽 -0xc（帧 0x28），清 int@0xd/short@0x11 后
    // get_int@0xd → LineFunc(0x2732)；get_short@0x11 → LineFunc(0x2734)。
    // ORIG store 形 `movl $0x0,0xd(%eax)`/`movw $0x0,0x11(%eax)` 单级 disp=
    // typed 字段直存（技巧14；char*+0xd 反产 add 两步覆写形）。
    MSG_CHANGE_LETTER_STAT* m = reinterpret_cast<MSG_CHANGE_LETTER_STAT*>(&msg);
    m->m_mailIdx = 0;
    m->m_letterStat = 0;
    if (!packet.get_int(m->m_mailIdx))
        return LineFunc(0x2732, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(m->m_letterStat))
        return LineFunc(0x2734, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}