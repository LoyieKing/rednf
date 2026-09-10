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
        if (!(G_GameWorld()->IsIntegratedPvPBaseChannel() &&
              G_CDataManager()->GetPvPChannelType() == 0))
        {
            getPVPResultW()->m_totalWinCount += 1;
        }
    }
    else
    {
        // ORIG：整合 PvP 基础频道且渠道类型 0 → 不计败场。
        if (!(G_GameWorld()->IsIntegratedPvPBaseChannel() &&
              G_CDataManager()->GetPvPChannelType() == 0))
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
    int battleMode = room->get_pvp_battle_mode();
    bool rewardMode = (battleMode == 4) || (room->get_pvp_battle_mode() == 5);
    if (rewardMode && out != 0)
    {
        unsigned int mobReward = 0;
        int level = get_charac_level();
        G_CDataManager()->get_mob_reward(level, mobReward);
        *out = (unsigned int)((getPVPResultR()->m_winPoint - beforeWinPoint) *
                              (int)mobReward);
        *out = *out + (unsigned int)gainPowerWarRewardExp((int)*out);
    }
    send_pvp_record();
    SetReliablePerson(false);
    SetOverTradeGoldCount(0);
