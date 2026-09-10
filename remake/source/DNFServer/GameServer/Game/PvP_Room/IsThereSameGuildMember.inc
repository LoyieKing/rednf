// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::IsThereSameGuildMember（原 TU 第 2949 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-7 N95/N82] ORIG 0x085da5f4：连续槽清零组 -0x20..-0x50，4.4 槽组整体深移 0x18（-0x2c..-0x54）+ 帧差；P167 CLEAR ⇒ 槽分配策略封顶。
// [b28d-solo-PvP_Room] 本轮：P225 推广 —— else 分支 `(p!=0)&&(k=guildkey(),k!=0)` 拆嵌套 if，
// 消 `mov$1/jmp/mov$0/test;je` bool 物化岛（139→134=ORIG）；残余 N82/N4 槽号 + scaled 折叠
// （ORIG `mov -0x40(%ebp,%ebx,4)` vs 4.4 `lea 0x4(%eax),%ebx` + scaled）。
// [SweepPvPRoom] 复核 gap=0（134=134）：ORIG 的 `-0x40/-0x50(%ebp,%ebx,4)` 是同一
// local_54[8] 数组的 `[i+4]`/`[i]` disp 折叠（Ghidra 同证单数组）；4.4 用
// `lea 0x4(%eax),%ebx` 再 scaled ⇒ N4/P30 伪影。另 ORIG 首初始化 `movl $0x4,-0x2c`
// 为死局部（Ghidra local_30=4，后续未读，P171/P232 族）⇒ 无源码解，封顶。
// [G8InvenPvP G8PvPAll 2026-09-10] P311 成员数组视图（旧 N4/P30「无源码解」判词待本轮实测）：
// `int local_54[8]` + 下标 `[i+4]` 改 `PvPGuildKeyView{m_team2[4] @低址, m_team1[4] @高址}`
// 具名成员数组访问。ORIG 直证单条 scaled disp 且索引不含 +4：写 0x085da69f
// `mov %eax,-0x40(%ebp,%ebx,4)`、读 0x085da723/0x085da72a `-0x40/-0x50(%ebp,%eax,4)`。
// 预期消 2 处 `lea 0x4(%eax),%ebx`+scaled 两段（-2 条；若 4.4 对偏移 0x10 成员数组仍拆
// 两段则中性，同 time_out 先例）。同址同 sizeof，清零序 [4..7]→[0..3] 不动。
bool PvP_Room::IsThereSameGuildMember()
{
    if ((m_pvpBattleMode == 2) || (m_pvpBattleMode == 3))
    {
        int local_dead;
        local_dead = 4;
        (void)local_dead;
        // [G8InvenPvP 2026-09-10 · P311 本件实测中性，已回滚] 曾试把
        // `int local_54[8]` + `[i+4]` 改成两个 4-int 具名成员数组视图
        // （ORIG 0x085da69f/0x085da723 单条 scaled disp）；编译后 ndiff 仍 58、
        // 指令 136=136 逐条不变 ⇒ 4.4 对偏移 0x10 的成员数组同样拆两段，
        // 维持 `int[8]` + 常量下标形（N4/P30 封顶）。
        int local_54[8];
        local_54[4] = 0;
        local_54[5] = 0;
        local_54[6] = 0;
        local_54[7] = 0;
        local_54[0] = 0;
        local_54[1] = 0;
        local_54[2] = 0;
        local_54[3] = 0;
        int local_2c = 0;
        int local_28 = 0;
        for (int local_24 = 0; local_24 < 8; local_24 = local_24 + 1)
        {
            if (P_TEAMS(local_24) == 1)
            {
                CUser* local_34 = P_SEATS(local_24);
                if (local_34 != 0)
                {
                    local_54[local_2c + 4] = local_34->get_charac_guildkey();
                    local_2c = local_2c + 1;
                }
            }
            else if (P_TEAMS(local_24) == 2)
            {
                CUser* local_34 = P_SEATS(local_24);
                if (local_34 != 0)
                {
                    local_54[local_28] = local_34->get_charac_guildkey();
                    local_28 = local_28 + 1;
                }
            }
        }
        if ((local_2c == 0) || (local_28 == 0))
        {
            return 0;
        }
        for (int local_20 = 0; local_20 < local_2c; local_20 = local_20 + 1)
        {
            for (int local_1c = 0; local_1c < local_28; local_1c = local_1c + 1)
            {
                if (local_54[local_20 + 4] == local_54[local_1c])
                {
                    return 1;
                }
            }
        }
    }
    else
    {
        int local_18 = 0;
        int local_14;
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            CUser* local_34 = P_SEATS(local_10);
            // [b28d-solo-PvP_Room] P225 推广：ORIG 0x085da5f4 为两段独立
            // `cmpl $0; je <next>` 直跳（无 setcc/bool 物化岛）；`&&`+逗号表达式
            // 在 4.4 恒物化成 `mov $1; jmp; mov $0; test %al; je`（+5 条）⇒ 拆嵌套 if。
            if (local_34 != 0)
            {
                local_14 = local_34->get_charac_guildkey();
                if (local_14 != 0)
                {
                    if (local_18 == local_14)
                        return 1;
                    local_18 = local_14;
                }
            }
        }
    }
    return 0;
}
