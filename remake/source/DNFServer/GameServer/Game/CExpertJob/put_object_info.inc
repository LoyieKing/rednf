// ORIG 0x85d1cc2：m_user 为空时走 cMyTrace 断言并返回 0；
// 否则 put_byte(0) + put_disjoint_machine_info 后返回 1。
bool CDisjointMachine::put_object_info(PacketGuard& p)
{
    if (m_user == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x33, 0);
        // [N5F 2026-09-11 §6.5 串盲区·真缺陷] ORIG 0x085d1cf5
        // `movl $"NOTEXIST_ERROR : disjoint_machine_user(null)",0x4(%esp)`。
        // 旧串为本仓自造文本；ORIG .rodata 实读（offset 13073056=0x0C7B9A0）只有
        // "NOTEXIST_ERROR : disjoint_machine_user(null)"，且全二进制无旧串。
        // AE 口径不比较串内容 ⇒ 此前四数不可见。prett 串/行号 0x33/level 0 与
        // ORIG 逐字节一致，勿动；本改只换字面量，指令数不变（31=31）。
        trace("NOTEXIST_ERROR : disjoint_machine_user(null)");
        return false;
    }
    p.put_byte(0);
    put_disjoint_machine_info(p);
    return true;
}