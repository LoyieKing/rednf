// OnConnectP2PTimeout ORIG T 0x8564d3a 报告 OnConnectP2PTimeout.md
// [b28 判词·N] AE-ndiff=2：唯一真差 = _ReturnTeam 实参装载中 mgrTeam 槽 reload 与
// `movl $0x0,0xc(%esp)` 的先后交换——4.1 值先载、4.4 常量先写（P75/N82附 装载时刻
// 跨版本调度）。探针 /tmp/l2_p3.cpp w1-w7（具名/拆分/语句表达式/cast 全形）不可翻；
// 禁 register 绑定（P0/N96）。其余逐条同。
// b28d-solo-CFairMatchRooms 判词：IDENT——P211/P274：局部 unsigned mgrTeam 与
// _ReturnTeam 的 int 形参异型，令 4.4 先求值第 3 参再写第 4 参常量（探针
// /tmp/fm_p3.cpp w31）；call 差集 5=5。fresh ndiff=0（52=52）。
void CFairMatch::OnConnectP2PTimeout(PvP_Room* room)
{
    if (m_field04 == 4) {
        _SendMatchStatePacket(4, 2);
        // b28d-solo-CFairMatchRooms：局部 unsigned + _ReturnTeam 形参 int 异型
        // （P211/P274 旋钮）令 4.4 先求值第 3 参再写第 4 参常量 —— 复现 ORIG 4.1 的
        // `mov -0x1c(%ebp),%eax; movl $0x0,0xc(%esp); mov %eax,0x8(%esp)` 装载序
        // （探针 /tmp/fm_p3.cpp w31）。
        unsigned int mgrTeam = room->get_manager_team();
        _ReturnTeam(room, mgrTeam, static_cast<CUser*>(0));
        if (mgrTeam != 1) {
            m_teams[0] = m_teams[1];
        }
        m_teams[0].refresh();
        m_teams[1].reset();
    }
}