// ---------------------------------------------------------------------------
// CParty::SetPlayResult —— ORIG T 0x085b2412（报告 SetPlayResult.md）：
//   结算一个成员：写排行/最佳时间、发送结算、清奖励、统计缺口告警。
// ---------------------------------------------------------------------------
// [b28d-solo-CPartyDepsD2] 判词 N（ORIG 0x085b2412，N80/N82 族）：
// 已修：①重复 get_dungeon_clear_state 删（ORIG 单次）；②m_field42→m_field48
// 真 bug（ORIG 0x085b251b 写 +0x48=0x183c，旧码写 +0x42 错位）；③resultSet
// 指针局部改 P235 具名视角（消 add $0x17f4）；④trace 改 P227 临时链；
// ⑤SendResult 返回 (unsigned char) 收窄复现 ORIG 0x085b2595 `movzbl %al,%eax`。
// 残余（193 vs 192，gap 1）：
//  ① ORIG 0x085b2457 `mov 0x330(%eax),%edx` 为**死读**（m_battleData.m_field4，
//     值随即被 0x085b245d 的 m_field0 装载覆盖、全函数无消费者）⇒ 4.1 对
//     dead local 保留 load 但不落槽；4.4 三形实测均不可造（探针
//     /tmp/probe_d2.cpp fA/fD 裸表达式+`(void)` 被 DCE；fB/fC 具名/volatile
//     局部多 store；/tmp/probe_d2b.cpp fK/fL volatile 取址多 `add $0x4`）⇒ 封 N。
//  ② SendResult 段 ORIG `lea 0x17f4(%eax),%edx` vs 4.4 `add $0x17f4,%eax`
//     （同址同指令数，union 内无 GameResultSet 完整类型视角可直访；P235 需改
//     CParty.h union 加非平凡成员，风险 > 收益）⇒ N。
// [b28d-SweepCPartyDepsD2-2] 真 bug 修复：trace 的 GetNoSend 实参序原写
// (3,2,1,0) ⇒ (%s,%s,%s,%s) 打印整体颠倒；ORIG 0x085b26a6 段证明 arg3..arg6
// = (0,1,2,3)（-0x64=(3)→arg6 / edi=(0)→arg3）。改源码为 (0,1,2,3) 后
// 4.4 右起求值产 call 3→2→1→0、槽 -0x64/-0x60/-0x5c/edi 与 ORIG 逐条一致
// （原判「varargs 求值序不可翻」作废——两侧同为 cdecl 右起，差异实为实参序）。
// 语义/调用集齐，byte 层封顶（仅余上述残余 ①②）。
// [b28d-round1-L24] **DIFF→AE**（193=193）：残余②的 dead 读真因 = 原码是 64 位
// union 成员的 int 截断 `(int)bd(this).m_clearPlayedTime`（探针 /tmp/probe_ll.cpp
// A 形：`mov 0x4(%eax),%edx; mov (%eax),%eax`，高半进 edx 后丢弃）；旧码
// `(int)bd(this).m_field0` 只读低半 ⇒ 少 1 条。改用 union 视角即逐条全等。
bool CParty::SetPlayResult(CUser* user)
{
    // [b28d-solo-CPartyDepsD2] ORIG 0x085b2412 单次 get_dungeon_clear_state +
    // `cmp$1;sete;test;je<共享尾>`（P222 正向包裹 + 尾 return 1）；resultSet
    // 无具名指针槽（P183：全点内联 this+0x17f4）；槽 -0x3c..-0x20 照抄。
    if (get_dungeon_clear_state() == 1)
    {
        int step = (int)set_play_result(user, m_gameResultSet);       // -0x3c
        // [b28d-solo-CPartyDepsD2] ORIG 0x085b2457 `mov 0x330(%eax),%edx;
        // mov 0x32c(%eax),%eax` = 64 位 union 整体装载后取低 32 位（P180 视角，
        // 探针 /tmp/probe_ll.cpp A 形）；写 m_field0 单半不会出该 edx 装载。
        const int clearTime = (int)bd(this).m_clearPlayedTime;        // -0x38
        const int dungeonIndex = (int)bf(this).get_dungeon_index();   // -0x34
        const int dungeonDiff = (int)bf(this).get_dungeon_diff();     // -0x30
        int avgTime = (int)G_CDataManager()->getDungeonClearAverageTime(
            dungeonIndex, getStartGamePartyCount(),
            (ENUM_DUNGEON_DIFFICULTY)dungeonDiff);                    // -0x2c
        const int prevField9 = (int)(signed char)m_gameResultSet.m_field9; // -0x28
        const int bonus = (int)G_CDataManager()->getClearTimeBonusPoint(
            avgTime, clearTime);                                      // -0x24
        const int rankPoint = G_CDataManager()->calRankPoint(
            bonus, prevField9);                                       // -0x20
        // [b28d-solo-CPartyDepsD2] 真 bug 修正：ORIG 0x085b251b 写 0x183c =
        // GameResultSet+0x48 = GameResultType::m_field48（旧 m_field42 落在
        // +0x42=0x1836，字段错位；clear_reward 读同一 +0x48）。P235 具名视角。
        m_gameResultSet.m_field48 =
            (unsigned short)G_CDataManager()->getRankIndex(rankPoint);
        m_gameResultSet.m_fieldc = (int)clearTime;
        m_gameResultSet.m_field10 = (unsigned char)bonus;
        if (!bf(this).isQuestMaze())
            setBestClearTime(dungeonIndex, dungeonDiff, clearTime);
        // ORIG 0x085b2595 `movzbl %al,%eax; mov %eax,-0x1c`（SendResult 单字节
        // 返回、int 接收）⇒ 调用点显式收窄（P3/探针 /tmp/p_movzbl.cpp 形 a）。
        int sendOk = (unsigned char)DisPatcher_SetPlayResult::SendResult(
            (CParty*)this, m_gameResultSet, step);
        if (sendOk == 0)
            return 0;
        clear_reward((int)m_gameResultSet.m_field48);
        set_dungeon_clear_state(2);
        insert_time_clear_dungeon_reward_msg(0x1e);
        if (get_member_count() != GetSetPlayResultCount())
        {
            // P227 全表达式临时链：ORIG 0x085b2612 求值序 GetNoSend(3/2/1/0)
            // 落槽 -0x64/-0x60/-0x5c/edi，压参 +0x1c/-0x18/+0x14/+0x10
            // = arg6/arg5/arg4/arg3 ⇒ 原码实参序为 (0),(1),(2),(3)
            // （4.1/4.4 同按 cdecl 右起求值）。旧码写 (3),(2),(1),(0) 是
            // **真 bug：(%s,%s,%s,%s) 打印顺序整体颠倒**（ORIG 0x085b26a6
            // 段：-0x64=(3)→arg6、edi=(0)→arg3）。
            cMyTrace(__PRETTY_FUNCTION__, 0x3261, 0)(
                "DUNGEON_CLEAR_BUG_CHECK total(%d),recv(%d), (%s,%s,%s,%s)",
                get_member_count(), GetSetPlayResultCount(),
                GetNoSendSetPlayResultUser(0), GetNoSendSetPlayResultUser(1),
                GetNoSendSetPlayResultUser(2), GetNoSendSetPlayResultUser(3));
        }
    }
    return 1;
}