// ---------------------------------------------------------------------------
// ORIG 0x08636555（报告缺失；objdump 0x08636555..0x086365d8 逐指令核实）：
//   key = p4 + 0x33
//   room = G_CGameManager()->GetPvp(p1, (CUser*)0, key)
//   room == 0 → return 0
//   if (!room->CheckTimerKey((TIMER_MESSAGE)p4, p2)) return 0
//   room->HandleTimerUserRevive(p4)
//   return 1
// （p3 在 ORIG 中未被读取。）
int PvPDieUserRevive(int p1, int p2, unsigned int p3, int p4)
{
    (void)p3;
    // P134/P24：p4+0x33 两次均作内联实参（首用跨 G_CGameManager call 由
    // callee-saved 缓存，二次用点重物化），勿具名局部（会多槽丢 push ebx）。
    PvP_Room* room = G_CGameManager()->GetPvp(p1, static_cast<CUser*>(0), p4 + 0x33);
    if (room == 0)
        return 0;
    if (!room->CheckTimerKey((TIMER_MESSAGE)(p4 + 0x33), p2))
        return 0;
    room->HandleTimerUserRevive(p4);
    return 1;
}