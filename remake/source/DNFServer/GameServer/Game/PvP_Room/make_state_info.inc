// [b28d-7 P75] ORIG 0x085dbd3a：receiver 先载（`mov -0xc(%ebp),%eax` 于常量槽 store 之前），4.4 恒反序（P75 跨版本装载时刻），P167 CLEAR。
// [b28d-solo-PvP_Room] P75: ORIG 0x085dbd3a receiver 先载/常量槽 store 序跨版本调度
// [SweepPvPRoom] 残余 1 条 = ORIG 0x085dbdc4 `mov %eax,%edx`（m_pvpBattleMode 实参
// 中转；同段其余成员走 `mov disp(%eax),%edx` 单段）⇒ P75/N18a 装载序封顶；short 实参
// `(int)` cast 已删（P200，ORIG `movswl %ax,%edx` 复现）。
// [L11 复核 2026-09-11 · 封顶] 同族：61=61、差异为 P75 装载序（receiver 先载 vs 实参先
//   store）+ %edx/%eax 角色 ⇒ 跨版本固有，勿再烧。
int PvP_Room::make_state_info(char* buf)
{
    InterfacePacketBuf* local_f4 = (InterfacePacketBuf*)buf;
    local_f4->put_header(0, 0x2a);
    local_f4->put_short(m_index);
    local_f4->put_byte(m_state);
    local_f4->put_byte(m_managerSeat);
    // [SweepPvPRoom] P200：ORIG 0x085dbd6x `movswl %ax,%edx`（short 直传）；
    // 冗余 (int) cast 退化成 `cwtl`（gap 1）。
    local_f4->put_short(m_requestedMapNo);
    // [L10] P265 实测无效（同上 make_room_info 注释），记 N。
    local_f4->put_byte(m_pvpBattleMode);
    local_f4->put_int(((IMatch*)m_matching)->GetIndentification());
    local_f4->finalize(true);
}