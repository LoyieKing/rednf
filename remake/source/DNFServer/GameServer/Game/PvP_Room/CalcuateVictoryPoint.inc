// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::CalcuateVictoryPoint（原 TU 第 390 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-7 N82] ORIG 0x085da210：帧 $0x64 槽组 -0x28 起，4.4 帧 $0x74 槽组 -0x2c 起（不按 lifetime 复用，N82）；P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] N82: ORIG 0x085da210 帧 $0x64 槽组 -0x28 起 vs 4.4 $0x74 槽组 -0x2c 起
// [SweepPvPRoom] gap 260=260 归零（旧判 N82 的 gap 3 实为两处真语义 bug + 两处形态差）：
// ①两处内层循环索引 bug：`rank[local_2c]`/`rank[local_24]` → `rank[local_28]`/`rank[local_1c]`
//   （ORIG 0x085da294/0x085da400 `mov -0x24/-0x18(%ebp),%eax; shl $0x2; add rank; mov;
//   cmp -0x28/-0x20(%ebp),%eax` 实证索引是内层循环变量）；
// ②P0 链式形：local_34 读 local_18、local_14 = local_18 - 1 + 循环尾自减（ORIG
//   0x085da475..0x085da487 三条链式赋值）；
// ③P223/BM8 双岛形：两处 `if (P_SEATS==0) goto L1; if (&&) goto L2; {body} L1:; L2:;`
//   复现 ORIG 0x085da24b/0x085da285 与 0x085da399/0x085da3d3 两独立尾岛（嵌套 if/continue
//   被 4.4 jump-thread 合并成单岛，缺 2 条 jmp）。残余 = N82 槽号整体深 0x10（帧同 $0x64）。
void PvP_Room::CalcuateVictoryPoint(int* rank, int rankCount)   // [b28c-16 SIGFIX] ORIG 串 void
{
    if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
    {
        for (int local_2c = 0; local_2c < 8; local_2c = local_2c + 1)
        {
            // [SweepPvPRoom] P223/BM8 双岛形：ORIG 0x085da24b `je 085da363`（条件1 落
            // nop+jmp 岛）与 0x085da285 `jne 085da366`（条件2 落 nop 岛）为两个独立尾岛，
            // body 尾 0x085da361 `jmp 085da367` 跳过两岛；嵌套 if/continue 形被 4.4
            // jump-thread 合并成单岛（缺 2 条 jmp）。多源 goto 到连续尾标签复现。
            if (P_SEATS(local_2c) == 0)
                goto LAB_NEXT1_2C;
            {
                // [DualMisc] ORIG 0x085da2xx：&& 物化为寄存器临时后 jne 跳过（无 xor 取反），
                // 非具名 bool 局部（后者恒落栈 -0x3d、撑大帧 $0x74 vs ORIG $0x64）。
                if (({ (P_SEATS(local_2c) != 0) && (IsPvpObserver(local_2c) != 0); }))
                {
                    goto LAB_NEXT2_2C;
                }
                {
                    int local_28 = 0;
                    while (local_28 < rankCount)
                    {
                        // [SweepPvPRoom] 真语义 bug：ORIG 0x085da294..0x085da29f 为
                        // `mov -0x24(%ebp),%eax; shl $0x2; add rank基址; mov (%eax),%eax;
                        // cmp -0x28(%ebp),%eax`（= rank[local_28] == local_2c）⇒ 索引是
                        // 内层循环变量 local_28，旧形 `rank[local_2c]` 用外层座位号。
                        if (rank[local_28] == local_2c)
                            break;
                        local_28 = local_28 + 1;
                    }
                    if (local_28 == rankCount)
                    {
                        cMyTrace local_54(__PRETTY_FUNCTION__, 0xb7c, 5);
                        local_54("1 PvP_Room::CalcuateVictoryPoint Error, No Rank User Found i<%d> RankCount<%d>",
                                 local_2c, rankCount);
                        goto LAB_085da583;
                    }
                    int local_34 = local_28;
                    bool local_2d = m_userTable.GetWinTeam() == local_2c;
                    P_SEATS(local_2c)->m_winPointBuffer =
                        CalculateVictoryPointOfSinglePlay(local_34, local_2d, m_fieldBc);
                }
            }
LAB_NEXT1_2C: ;
LAB_NEXT2_2C: ;
        }
    }
    else
    {
        for (int local_24 = 0; local_24 < 8; local_24 = local_24 + 1)
        {
            // [SweepPvPRoom] P223/BM8 双岛形（同单挑分支）：ORIG 0x085da399 `je 085da56c`
            // 与 0x085da3d3 `jne 085da56f` 为两个独立尾岛、body 尾 0x085da56a `jmp 085da570`。
            if (P_SEATS(local_24) == 0)
                goto LAB_NEXT1_24;
            {
                // [DualMisc] ORIG 0x085da4xx：同单挑分支，&& 物化后 jne 跳过。
                if (({ (P_SEATS(local_24) != 0) && (IsPvpObserver(local_24) != 0); }))
                {
                    goto LAB_NEXT2_24;
                }
                {
                    int local_20 = GetTeamIndex(P_SEATS(local_24));
                    int local_1c = 0;
                    while (local_1c < rankCount)
                    {
                        // [SweepPvPRoom] 真语义 bug（同单挑分支）：ORIG
                        // 0x085da400..0x085da40b `mov -0x18(%ebp),%eax; shl $0x2; add;
                        // mov (%eax),%eax; cmp -0x20(%ebp),%eax` ⇒ rank[local_1c] == local_24。
                        if (rank[local_1c] == local_24)
                            break;
                        local_1c = local_1c + 1;
                    }
                    if (local_1c == rankCount)
                    {
                        cMyTrace local_44(__PRETTY_FUNCTION__, 0xb97, 5);
                        local_44("1 PvP_Room::CalcuateVictoryPoint Error, No Rank User Found i<%d> RankCount<%d>",
                                 local_24, rankCount);
                        goto LAB_085da583;
                    }
                    int local_18 = local_1c;
                    // [SweepPvPRoom] P0 链式形：ORIG 0x085da475..0x085da487 为
                    // `-0x14 = -0x18`、`-0x30 = -0x14`、`-0x10 = -0x14 - 1`（三条独立
                    // 读 local_1c 的形多 1 条且槽链不符）⇒ local_34 读 local_18、
                    // local_14 初始化即减 1，循环尾自减（4.1 for 形：init; jmp COND;
                    // BODY; subl; COND @0x085da4fc/0x085da500）。
                    int local_34 = local_18;
                    // [SweepPvPRoom-2] 真 bug（Main 高优先级排查）：原 while 形把自减放
                    // 循环体末尾 ⇒ `continue` 跳过自减（遇观察者座位死循环）；ORIG
                    // 0x085da4f3 `je +0x2ec` / 0x085da4fc `subl $0x1,-0x10` 增量在体后、
                    // COND(0x085da500) 之前（for 形）⇒ continue 恒执行增量。改 for 复现。
                    for (int local_14 = local_18 - 1; -1 < local_14; local_14 = local_14 - 1)
                    {
                        int local_10 = rank[local_14];
                        // [DualMisc] ORIG 0x085da6xx：同前，&& 物化后 jne 跳过（无 xor）。
                        if (({ (P_SEATS(local_10) != 0) && (IsPvpObserver(local_10) != 0); }))
                        {
                            continue;
                        }
                        if (GetTeamIndex(P_SEATS(local_10)) != local_20)
                        {
                            local_34 = local_34 - 1;
                        }
                    }
                    bool local_2d = m_userTable.GetWinTeam() == local_20;
                    P_SEATS(local_24)->m_winPointBuffer =
                        CalculateVictoryPointOfTeamPlay(local_34, local_2d, m_fieldBc / 2);
                }
            }
LAB_NEXT1_24: ;
LAB_NEXT2_24: ;
        }
    }
LAB_085da583:
    ;
}
