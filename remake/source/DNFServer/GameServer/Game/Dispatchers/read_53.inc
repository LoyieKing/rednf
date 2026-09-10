int Dispatcher_MemberSecede::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081cbb26：int@0x10（len）→ str@0x14（max 0x1e）
    // 单一局部槽 v（ORIG `mov 0x10(%ebp),%eax; mov %eax,-0xc` 两次回读：
    // get_int 前 `mov -0xc;add $0x10`、get_str 前 `mov -0xc;mov 0x10(%eax)` +
    // `mov -0xc;add $0x14`）。
    MSG_MEMBER_SECEDE* v = reinterpret_cast<MSG_MEMBER_SECEDE*>(&msg);
    if (!packet.get_int(v->m_nameLen))
        return LineFunc(0x2209, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(reinterpret_cast<char*>(v) + 0x14, 0x1e, v->m_nameLen))
        return LineFunc(0x220c, __PRETTY_FUNCTION__, 0, 0);
    return 0;
