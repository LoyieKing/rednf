// ---- OnEnterDisjointStore（报告，ORIG T 0x085d309a sz 0x101）----
// 包 0xf5：byte(1) byte(等级+1) int(修理费) int(耐久)。
int CDisjointer::OnEnterDisjointStore(CUser* user)
{
    PacketGuard packet;
    packet.put_header(1, 0xf5);
    packet.put_byte(1);
    packet.put_byte(get_disjoint_machine_grade() + 1);
    packet.put_int(get_disjoint_cost());
    // ORIG：owner/expand 无槽，链式内联（P52 单语句）。
    packet.put_int(((CCharacExpertJob*)m_machine.get_user()->GetCharacExpandData(
                        (ENUM_CHARAC_EXPAND_TYPE)0))
                       ->get_disjoint_machine_endurance());
    packet.finalize(true);
    user->Send(packet);
    return 0;
}