// 0x0865c678
// ORIG：PvP 结果结算。观战者直接返回；winPointBuffer(+0x796fc) 清零并入账；
// 胜/败计数（整合 PvP 基础频道且渠道类型==0 时不计）；额外胜点补发并记 WPAdd 日志；
// 战斗模式 4/5 且 out 非空时按 mob_reward×胜点增量 + gainPowerWarRewardExp 计算奖励。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865c678 | _ZN5CUser14add_pvp_resultEbPj
void CUser::add_pvp_result(bool flag, unsigned int* out)
{
    PvP_Room* room = (PvP_Room*)GetPVPRoom();
    if (room == 0)
        return;
    int seat = room->get_user_seat(this);
    if (room->IsPvpObserver(seat))
        return;
    lock();
    // b28d-G3C 2026-09-10（P183 去具名指针）：ORIG 0x0865c7?? `mov 0x8(%ebp),%eax;
    //   mov %eax,(%esp); call getPVPResultR; mov 0x58(%eax),%eax; mov %eax,-0x14(%ebp)`
    //   ——**链式直读**（无中间 `const PvpResultType* before` 槽）。我方原具名独占
    //   -0x24 槽（帧 0x44→0x54），残差 +0x10 的主因。
    int beforeWinPoint = getPVPResultR()->m_winPoint;
    int delta = m_winPointBuffer;
    m_winPointBuffer = 0;
    // b28d-G3C 2026-09-10（P59/P222 分支反写）：ORIG 0x0865c74a 段是
    //   `cmpb $0x0,-0x2c(%ebp); je +0xd9` 后紧跟 `addl $0x1,-0x10(%ebp)`（delta+1）
    //   ⇒ **then = flag 真**；我方原写 `if (!flag) {...} else {delta+1...}` 产
    //   `movzbl -0x3c; xor $0x1; test %al,%al; je`（3 条 + 取反）。两分支互换。
    if (flag)
    {
        delta = delta + 1;
        // 2026-09-11 CuA（P59/P222 守卫包裹形 + 去 xor 物化）：ORIG 0x865c70b..
        //   0x865c74f 为 `call IsIntegratedPvPBaseChannel; test %al,%al; je +0x1a` /
        //   `call GetPvPChannelType; test %eax,%eax; jne +0x14` / `mov $0x1,%eax; jmp` /
        //   `mov $0x0,%eax; test %al,%al; jne <join>` —— **无 `xor $0x1`**、then 为空、
        //   body 在 else。原写 `!(A && B==0)` 令 4.4 产 `xor $0x1,%eax` + 极性镜像
        //   （我方 index 44/67 两处）。探针 /tmp/P5/p_guard h.cpp c1：正向空 then +
        //   else body 与 ORIG 逐条同形（含 mov $1/mov $0 物化）。
        if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
            G_CDataManager()->GetPvPChannelType() == 0)
        {
        }
        else
        {
            getPVPResultW()->m_totalWinCount += 1;
        }
    }
    else
    {
        // ORIG：整合 PvP 基础频道且渠道类型 0 → 不计败场。
        if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
            G_CDataManager()->GetPvPChannelType() == 0)
        {
        }
        else
        {
            getPVPResultW()->m_totalLoseCount += 1;
        }
    }
    getPVPResultW()->m_winPoint += delta;
    m_historyLog.WPAdd(getPVPResultR()->m_winPoint, delta, eWPAddReason_0);
    int extra =
        room->get_extra_win_point(getPVPResultW()->m_winPoint - beforeWinPoint);
    if (extra != 0)
    {
        getPVPResultW()->m_winPoint += extra;
        m_historyLog.WPAdd(getPVPResultR()->m_winPoint, extra, eWPAddReason_0);
    }
    unlock();
    // b28d-G5（P0/P183 删多余具名局部）：ORIG 0x0865c857 段
    //   `mov -0x1c(%ebp),%eax; mov %eax,(%esp); call get_pvp_battle_mode;
    //    cmp $0x4,%eax; je +0x1ff` + 岛 `mov $1/mov $0; test %al,%al; je +0x280`
    //   ——battleMode / rewardMode **均无槽**（内联 || 链）；我方原具名占
    //   -0x14(4B) 与 -0xd(1B) 两槽 ⇒ 帧 0x54 vs ORIG 0x44（+0x10）。
    if ((room->get_pvp_battle_mode() == 4 ||
         room->get_pvp_battle_mode() == 5) &&
        out != 0)
    {
        unsigned int mobReward = 0;
        // ORIG 0x0865c89f `mov %eax,%ebx`（level 无槽，跨 G_CDataManager call 保活）
        register int level = get_charac_level();   // 裸 register（非绑定，P0 合规）
        G_CDataManager()->get_mob_reward(level, mobReward);
        *out = (unsigned int)((getPVPResultR()->m_winPoint - beforeWinPoint) *
                              (int)mobReward);
        *out = *out + (unsigned int)gainPowerWarRewardExp((int)*out);
    }
    send_pvp_record();
    SetReliablePerson(false);
    SetOverTradeGoldCount(0);
}