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
// [L11 已**解** 2026-09-11 · ndiff 260→8] 旧判「槽号不可翻」推翻。真因 = **4.4 按
//   lifetime 不复用同名局部槽**（4.1 复用）：三旋钮 ——
//   ① 单挑/团战两处外层循环变量（local_2c/local_24）hoist 到各自块首：260→252；
//   ② 同名 `local_34`/`local_2d`（两分支各声明一次）**提为函数级共用**：252→**8**
//      （残 8 行 = 两个 cMyTrace 对象槽互换：ORIG 单挑 cMyTrace@-0x50/团战@-0x40，
//       4.4 反之（先声明者更浅）⇒ 4.1↔4.4 expand 顺序差，帧同为 $0x64、槽集完全一致；
//       ctor 参数 0xb7c 直证归属：ORIG 该 ctor 用 `lea -0x50`）。
//   ③ 团战 7 局部 hoist：324 **劣化已回滚**。范式 **P-L11d**：同名局部提函数级促 4.4 复用槽。
void PvP_Room::CalcuateVictoryPoint(int* rank, int rankCount)   // [b28c-16 SIGFIX] ORIG 串 void
{
    // [L11 2026-09-11 · 第3旋钮] ORIG 两分支复用同一槽（bool -0x29 共 4 次引用、
    // 计数值 -0x30 共 5 次）而 4.4 按 lifetime 不复用（另生 -0x11 / -0x34）⇒ 把同名
    // local_34/local_2d 提为函数级共用，促 4.4 复用。
    int local_34;
    bool local_2d;
    if ((m_pvpBattleMode == 1) || (m_pvpBattleMode == 4))
    {
        int local_2c;
        int local_28;
        for (local_2c = 0; local_2c < 8; local_2c = local_2c + 1)
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
                    local_28 = 0;
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
                        // [WorldLane 2026-09-11 · P221] 具名 cMyTrace 两形互试：本件 ORIG
                        // 单挑 cMyTrace@-0x50 / 团战@-0x40（后声明占更深槽），4.4 具名形
                        // 相反 ⇒ 改「构造即调用」全表达式临时链翻正槽方向。
                        cMyTrace(__PRETTY_FUNCTION__, 0xb7c, 5)(
                            "1 PvP_Room::CalcuateVictoryPoint Error, No Rank User Found i<%d> RankCount<%d>",
                            local_2c, rankCount);
                        goto LAB_085da583;
                    }
                    local_34 = local_28;
                    local_2d = m_userTable.GetWinTeam() == local_2c;
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
        int local_24;
        for (local_24 = 0; local_24 < 8; local_24 = local_24 + 1)
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
                        // [WorldLane 2026-09-11 · P221] 同单挑分支：全表达式临时链翻槽方向。
                        cMyTrace(__PRETTY_FUNCTION__, 0xb97, 5)(
                            "1 PvP_Room::CalcuateVictoryPoint Error, No Rank User Found i<%d> RankCount<%d>",
                            local_24, rankCount);
                        goto LAB_085da583;
                    }
                    int local_18 = local_1c;
                    // [SweepPvPRoom] P0 链式形：ORIG 0x085da475..0x085da487 为
                    // `-0x14 = -0x18`、`-0x30 = -0x14`、`-0x10 = -0x14 - 1`（三条独立
                    // 读 local_1c 的形多 1 条且槽链不符）⇒ local_34 读 local_18、
                    // local_14 初始化即减 1，循环尾自减（4.1 for 形：init; jmp COND;
                    // BODY; subl; COND @0x085da4fc/0x085da500）。
                    local_34 = local_18;
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
                    local_2d = m_userTable.GetWinTeam() == local_20;
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