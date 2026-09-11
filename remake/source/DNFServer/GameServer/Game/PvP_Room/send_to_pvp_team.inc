// [b28d-solo-PvP_Room] 两循环各用「P226 语句表达式包前两条件 + 嵌套 if 包第三条件」
// 复现 ORIG 0x085dbeb9/0x085dbf36 的「先物化 bool 岛、再比较第三条件」布局。
// [SweepPvPRoom] P223 变体（b28c C28L19 裸早返产尾岛）：ORIG 0x085dbe73
// `jle 085dbf7c`（else 岛 = nop + jmp 085dbf80）与 0x085dbf7a `jmp 085dbf80`
// （第二循环出口）、0x085dbf7f nop（local_18==0 岛）共 2 条 jmp；旧 if 包裹形被
// 4.4 epilogue-threading 合并（旧判 N121 的 2 条缺口）。改裸早返 `if (mode<=1) return;`
// 复现 else 岛 + body 尾 jmp。
void PvP_Room::send_to_pvp_team(PacketGuard& guard, CUser* user)
{
    // [b28d-solo-PvP_Room] ORIG 0x085dbe64 用 jle（有符号）比较；m_pvpBattleMode
    // 字段为 unsigned（set_pvp_mode store 序需要，P211），此处 (int) 收窄比较域。
    if ((int)m_pvpBattleMode <= 1)
        return;
    {
        int local_18 = 0;
        for (int local_14 = 0; local_14 < 8; local_14 = local_14 + 1)
        {
            // [b28d-solo-PvP_Room] ORIG 0x085dbeb9：前两条件先物化 bool 岛
            // （mov$1/jmp/mov$0/test/je），随后才是 `P_SEATS==user` 比较 ⇒
            // 语句表达式包前两条件 + 嵌套 if 包第三条件（直接 && 链恒把物化推迟
            // 到最后，物化位置错）。
            if (({ (P_SEATS(local_14) != 0) &&
                   ((char)IsPvpObserver(local_14) != 1); }))
            {
                if (P_SEATS(local_14) == user)
                {
                    local_18 = P_TEAMS(local_14);
                    break;
                }
            }
        }
        if (local_18 == 0)
            return;
        // [SweepPvPRoom-2] P223 变体（裸早返产尾岛）：`if (local_18 == 0) return;`
        // 复现 ORIG 0x085dbf7f nop 岛（je 落点）+ 0x085dbf7a jmp（第二循环出口）
        // 双岛形（94=94 AE）；旧 `if (local_18 != 0) { … }` 包裹形被 4.4 合并成一个
        // 尾岛（92 条）。
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            // 同第一循环：ORIG 0x085dbf36 段亦为「前两条件物化岛 + 第三条件
            // 独立比较」⇒ 语句表达式 + 嵌套 if。
            if (({ (P_SEATS(local_10) != 0) &&
                   ((char)IsPvpObserver(local_10) != 1); }))
            {
                if (P_TEAMS(local_10) == local_18)
                {
                    P_SEATS(local_10)->Send(guard);
                }
            }
        }
    }
}