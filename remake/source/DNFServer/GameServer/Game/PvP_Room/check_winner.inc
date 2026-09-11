// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::check_winner（原 TU 第 1031 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================
// [b28d-7 N82] ORIG 0x085d96ec：大帧 `add $<A>,%esp`（槽组 -0x28..-0x68），4.4 `sub $0x90` + 槽组深移；P167 CLEAR ⇒ N82 槽复用策略封顶。
// [b28d-solo-PvP_Room] N82: ORIG 0x085d96ec 大帧槽组 -0x28..-0x68 vs 4.4 -0x90；P167 CLEAR
bool PvP_Room::check_winner()
{
    // ORIG 0x085d98ea（274 条；本侧 290，+16 全为 N 类形态差，台账豁免）：
    //  - 帧调整 4.1 `add $0xffffff80,%esp`（负常量折叠）vs 4.4 `sub $0x90`；
    //    帧洞 0x10（outgoing frame N）；
    //  - local_6c[P_TEAMWIN+3] 索引 4.1 单 lea 折叠、4.4 拆 mov+add 两段
    //    （N18a/N76）；bVar 常量岛 `mov$1/mov$0/test;je`×3（setcc-fold）；
    //  - mode5 段 &m_deathMatch 先期 lea 缓存（P52 callee-saved），4.4 实参处
    //    现算。GetRanking/insert 调用链与错误码逐段核对无语义缺口。
    // [b28d-t3-PoolBigThree] 求值次数形实测（保持 += 单语句形，insn 290→288）：
    // `local_6c[T(i)+3] += 1` 在 4.4 仍产双求值（`lea 0x3(,%eax)` + 重算 `add $0x3`
    // 两段，ORIG 0x085d9799 单 %eax scaled 复用）⇒ 4.1 -O0 表达式树 CSE 不存在于
    // 4.4，具名局部会引入 ORIG 无的索引槽（反劣）。N18a/N76 判词维持，ndiff 171。
    // [L10b 2026-09-10] 三处改善（289→282，ORIG 277）：
    //  ① **P311 成员数组视图**：`int local_6c[6]` + 下标 `[T+3]` 改
    //     `struct PvPWinCountView { int m_win[3]; int m_teamWin[3]; }` 成员数组访问 ⇒
    //     复现 ORIG 单条 scaled disp `mov -0x5c/-0x68(%ebp,%eax,4)`（旧形 4.4 恒拆
    //     `lea 0x3(%eax)` + 重算 add，-2 条）；清零/第二循环读取同址等价。
    //  ② **P231 裸 register**：mode5 段 `register int uVar2 = get_pvp_team_cnt();`
    //     ⇒ 复现 ORIG 0x085d99ab `call; mov %eax,%ebx` 跨 cMyTrace ctor/op() 保活；
    //  ③ P0 内联实参：第二次 `get_pvp_team_cnt()` 不再落槽（ORIG 0x085d99fb 直压）。
    //  残余 = 4.1 表达式 CSE（每处 += 重算索引 4 条，两处 8 条，N18a/N76）+ bool
    //  物化（checkGameOver* 已 bool 化，P292 档①；余 single/team 各 1 段）。
    if (m_fieldC0 != 0)
    {
        return 0;
    }
    if ((m_pvpBattleMode == 2) || (m_pvpBattleMode == 3))
    {
        lock();
        // [L10b] P311 成员数组视图：ORIG 0x085d9799/0x085d97c7 为单条
        // `mov -0x5c/-0x68(%ebp,%eax,4),%edx; add $1; mov %edx,...`（scaled disp 且
        // **索引不含 +3**）⇒ 原码是 `{int m_win[3]; int m_teamWin[3];}` 视图对象的
        // 成员数组访问（m_win@-0x68 组、m_teamWin@-0x5c 组）。旧 `int local_6c[6]` +
        // 下标 `[T+3]` 使 4.4 恒拆 `lea 0x3(%eax)` + 重算 `add $0x3` 两段。
        struct PvPWinCountView
        {
            int m_win[3];        // local_6c[0..2]（ORIG -0x68 组；第二循环读取）
            int m_teamWin[3];    // local_6c[3..5]（ORIG -0x5c 组）
        };
        PvPWinCountView local_6c;
        local_6c.m_teamWin[0] = 0;
        local_6c.m_teamWin[1] = 0;
        local_6c.m_teamWin[2] = 0;
        local_6c.m_win[0] = 0;
        local_6c.m_win[1] = 0;
        local_6c.m_win[2] = 0;
        for (int local_2c = 0; local_2c < 8; local_2c = local_2c + 1)
        {
            if ((P_TEAMWIN(local_2c) != 0) && (P_TEAMWIN(local_2c) < 0xfe))
            {
                // [FL8PvPWorldLeague 2026-09-10] `+= 1` → 前置 `++`：4.4 对
                // `arr[成员读] += 1` 双求值下标（读/写各一次），`++arr[..]` 单求值
                // （time_out 同族实证 ndiff 99→85）。
                ++local_6c.m_teamWin[P_TEAMWIN(local_2c)];
                if (P_WIN(local_2c) != 0)
                {
                    ++local_6c.m_win[P_TEAMWIN(local_2c)];
                }
            }
        }
        int local_34 = 0;
        unsigned int local_30 = 0;
        for (int local_28 = 1; (int)local_28 < 3; local_28 = local_28 + 1)
        {
            if (0 < local_6c.m_win[local_28])
            {
                local_34 = local_34 + 1;
                local_30 = local_28;
            }
        }
        if (local_34 == 1)
        {
            for (int local_24 = 0; local_24 < 8; local_24 = local_24 + 1)
            {
                if (P_TEAMWIN(local_24) == local_30)
                {
                    P_WIN(local_24) = 1;
                }
                else
                {
                    P_WIN(local_24) = 0;
                }
            }
        }
        unlock();
        if (local_34 == 1)
        {
            m_userTable.SetWinTeam(local_30);
            return 1;
        }
    }
    else if (m_pvpBattleMode == 4)
    {
        int local_20 = 0;
        for (int local_1c = 0; local_1c < 8; local_1c = local_1c + 1)
        {
            if ((P_SEATS(local_1c) != 0) && (P_TEAMS(local_1c) != 4))
            {
                local_20 = local_20 + 1;
            }
        }
        cMyTrace local_54(__PRETTY_FUNCTION__, 0xa3c, 0);
        local_54("PvP_Room::check_winner single : %d", local_20);
        // [L10b P292 档①] callee 已 bool 化 ⇒ 裸形（`if (f())`）直产 ORIG
        // 0x085d9950 `test %al,%al; je`；`!= 0` 会多一条比较。
        if (m_deathMatch.checkGameOverForSinglePlay(local_20))
        {
            m_deathMatch.checkWinnerForSinglePlay(m_users, m_winCheck, m_userTable);
            return 1;
        }
    }
    else if (m_pvpBattleMode == 5)
    {
        // [L10b] P231 裸 register：ORIG 0x085d99ab `call get_pvp_team_cnt; mov %eax,%ebx`
        // 跨 cMyTrace ctor/op() 保活于 callee-saved %ebx，且 0x085d99fb 的第二次
        // 调用结果**不落槽**直压实参（P0）⇒ 首值留 callee-saved + 次值内联实参。
        register int uVar2 = get_pvp_team_cnt();
        cMyTrace local_44(__PRETTY_FUNCTION__, 0xa46, 0);
        local_44("PvP_Room::check_winner team  : %d", uVar2);
        if (m_deathMatch.checkGameOverForTeamPlay(m_users, get_pvp_team_cnt(), m_teamWin))
        {
            m_deathMatch.checkWinnerForTeamPlay(m_users, m_winCheck, m_teamWin, m_userTable);
            return 1;
        }
    }
    else
    {
        lock();
        int local_18 = 0;
        // [J8PvPRoom 2026-09-11] P328：ORIG 0x085d9a91 只清 -0x14/-0xc（local_18/
        // local_10），-0x10（local_14）**无清零指令**（ORIG 0x085d9abf 才首次写
        // `mov %eax,-0x10(%ebp)`）⇒ 源码声明无初值，删 `= 0` 消一条 `movl $0x0`。
        int local_14;
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            if ((P_WIN(local_10) != 0) && (P_TEAMS(local_10) != 4))
            {
                local_18 = local_18 + 1;
                local_14 = local_10;
            }
        }
        unlock();
        if (local_18 == 1)
        {
            m_userTable.SetWinTeam(local_14);
            return 1;
        }
    }
    return 0;
}