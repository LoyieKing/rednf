// ---------------------------------------------------------------------------
// CDisjointMachine getter（ORIG W 0x85d3e06 / 0x082347e0 / 0x85d3e12）
// P210：以下 CDisjointer 包装（grade/isMine）必须定义于 get_user/get_grade
// 之前，否则 4.4 继承 callee outgoing 帧塌缩（sub $0x4 vs ORIG $0x18）。
// ---------------------------------------------------------------------------
// get_disjoint_machine_grade（ORIG T 0x85d1f0a：movsbl m_machine.get_grade()）
int CDisjointer::get_disjoint_machine_grade()
{
    return (int)m_machine.get_grade();
}