// ORIG T 0x0855e4c4。ORIG 返回 1。
// 【N7 封顶】b26 lane19：虚调用 callee 寄存器 %ecx↔%eax 互换 + 实参装载序
// 跨版本差（ORIG 先槽 -0x10 后形参 0x10，4.4 反序——N80 族不可翻），33 条
// 长度/助记符全等。
// [b28d-solo-CLeagueMatch] NEAR 33/33·N7（S180 复验）：虚调用目标
// %ecx(ORIG 0x0855e4dc)↔%edx(4.4.7) + 实参装载序（P75）；33 条助记符逐条全等，
// 语义等价。
// [J9MgrTimerCLeagueMatch.V1 2026-09-11 终局] 本件唯一可翻路径 = fn 落 %ecx（ORIG
// 0x0855e4fc `mov (%eax),%ecx`）。本轮两旋钮：①`static_cast<IMatch*>(this)->OnLeaveRoom(...)`
// 使 4.4 产 `mov (%eax),%ecx`（fn 对齐 ✓）但接收者提前载入 %eax ⇒ 实参全改 %edx，
// 行差 6/6→9/9 **净劣**，已回滚；②cc1plus **4.4.6 后端** A/B（头集 4.4.6 + i486，
// 独立 /tmp/tu_CLeagueMatch_446.o）逐件读数与本 TU 全同（44/63/1/16）⇒ 后端维度
// 无解。根因 = i386 REG_ALLOC_ORDER 差（ORIG 首选 %ecx、4.4.7 首选 %edx），源码层
// 不可控【已试尽】。p167 2/2 CLEAR。
int CLeagueMatch::OnWalkOut(PvP_Room* room, int seat)
{
    int team = room->get_team(seat);
    CUser* user = reinterpret_cast<CUser*>(room->get_user_seat(seat));
    OnLeaveRoom(user, room, team, seat);                // vtable+0x2c 四参形态
    return 1;
}