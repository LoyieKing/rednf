// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::start_pvp（原 TU 第 3184 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-7 N82附] ORIG 0x085d8c3a：成员值经 callee-saved 跨 cMyTrace 构造保活（movzwl/movswl 两段），4.4 改槽中转（P227/P0 边界）；373/373 助记符全等、P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] 本轮：P227 三处 cMyTrace 改全表达式临时链（vararg 在 ctor 前求值升格
// callee-saved）、P46 switch 叶块序改 1/4→3→2/5、P59 三 case 两体交换（jle 直落 else）；
// 残余仅 P75/N7 实参装载时刻（local_14→%ebx 跨 receiver call、GetPVPMapCount 求值序）⇒ 封顶。
void PvP_Room::start_pvp()   // [b28c-16 SIGFIX] ORIG 串 void
{
    if (get_waiter_count() < 2)
    {
        goto LAB_085d91da;
    }
    {
        ((IMatch*)m_matching)->RequestMap(m_requestedMapNo,
                                          reinterpret_cast<PVP_BATTLE_MODE&>(m_pvpBattleMode));
        if ((m_requestedMapNo < 0) || (m_requestedMapNo > G_CDataManager()->m_mapList->GetPVPMapCount()))
        {
            // [b28d-solo-PvP_Room] P227：ORIG 0x085d8c3a 在 ctor call 之前
            // 右起求 vararg 并跨 ctor 保活 callee-saved（`movswl %ax,%ebx` 先于
            // ctor，`mov %ebx,0x8(%esp)` 后于 ctor）⇒ 全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0x880, 0)(
                "START_PVP request_map_index(%d)", (int)m_requestedMapNo);
            m_requestedMapNo = 0;
        }
        if (m_requestedMapNo == 0)
        {
            m_playMapNo = (short)SelectRandomMap();
            if ((m_playMapNo < 0) ||
                (m_playMapNo >= G_CDataManager()->m_mapList->GetPVPMapCount()))
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x88b, 0)(
                    "START_PVP select_map_index(%d)", (int)m_playMapNo);
            }
        }
        else
        {
            m_playMapNo = m_requestedMapNo;
        }
        if (G_GameWorld()->GetChannelType() == 6)
        {
            m_guildWarLog.SetpvpStartTime(GlobalData::s_systemTime_.getCurSec());
        }
        int local_18 = 0;
        PacketGuard local_54;
        for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
        {
            if (P_SEATS(local_10) != 0)
            {
                local_18 = local_18 + 1;
                P_WIN(local_10) = 1;
                P_TEAMWIN(local_10) = (unsigned char)P_TEAMS(local_10);
                m_relayMgr.OnChangeTeam(local_10, (int)P_TEAMWIN(local_10));
                P_SEATS(local_10)->reset_pvp_masterid_walkingout_me();
                local_54.clear();
                local_54.put_header(0, 0x2d);
                local_54.put_byte((int)m_playMapNo);
                local_54.put_byte(m_pvpBattleMode);
                local_54.finalize(true);
                P_SEATS(local_10)->Send(local_54);
                P_SEATS(local_10)->set_state((ch_state)7);
                P_SEATS(local_10)->getHackAnalyzer()->beginCollectHackInfo(P_SEATS(local_10));
                disconnect_detecter::GetInstanceDisconnectDetecter()->RegisterUser(
                    P_SEATS(local_10));
            }
        }
        Statistics::PVPPlay2((int)m_playMapNo, m_pvpBattleMode, local_18);
        calculate_pvp_point();
        m_fieldBc = get_waiter_count();
        m_state = 2;
        if (m_pvpBattleMode == 3)
        {
            m_relayMgr.OnStart();
        }
        if ((m_pvpBattleMode == 4) || (m_pvpBattleMode == 5))
        {
            m_deathMatch.OnStart();
        }
        m_battleStartSec = GlobalData::s_systemTime_.getCurSec();
        // [Lane10] P248 ①-④ 实测：`register int local_14` 令 4.4 少占 1 槽
        // （cMyTrace 槽 -0x44→-0x40）且 ndiff 24→109 反劣 ⇒ 回退，本件维持
        // P75/N7 装载时刻封顶（ORIG 0x085d90c6 `mov -0x10(%ebp),%ebx` 跨
        // GetInstanceCommonStatisticsMgr() call 保活 vs 4.4 call 后回读 %edx）。
        int local_14 = 0;
        switch (m_pvpBattleMode)
        {
        case 1:
        case 4:
            // [b28d-solo-PvP_Room] P59 双向布局：ORIG 0x085d8c3a 为 `cmpl $2; jle <T>`，
            // then 体（local_14=0）落 <T>、else 体（=1）直落 ⇒ 条件取反+交换两体。
            // P46 叶块顺序：ORIG 0x85d9080(1/4)→0x85d9098(3)→0x85d90b0(2/5) ⇒ case 3 提前。
            if (local_18 >= 3)
            {
                local_14 = 1;
            }
            else
            {
                local_14 = 0;
            }
            break;
        case 3:
            if (local_18 >= 3)
            {
                local_14 = 5;
            }
            else
            {
                local_14 = 4;
            }
            break;
        case 2:
        case 5:
            if (local_18 >= 3)
            {
                local_14 = 3;
            }
            else
            {
                local_14 = 2;
            }
            break;
        }
        // [b28d-solo-PvP_Room] P75/N7 封顶：ORIG 在 receiver call 前把 local_14 升格
        // %ebx 保活（`mov -0x10(%ebp),%ebx; call; mov %ebx,0x8(%esp)`）；P226 裸语句
        // 表达式实参实测无效（具名变量读取不升格）⇒ 跨版本装载时刻差。
        GetInstanceCommonStatisticsMgr()->IncreaseQuantity(
            (CCommonStatisticsMgr::ENUM_STATISTIC_CONTAINER)1, local_14);
        // [Lane10] P243/P66 求值序：ORIG 0x085d90f1..0x085d9110 先 call
        // GetPVPMapCount（结果留 %eax）再重读 m_playMapNo 到 %edx（`cmp %edx,%eax;
        // jle`，无 callee-saved 缓存）；旧形左操作数写成员使 4.4 把 m_playMapNo
        // 升格 %ebx 跨 2 call 保活（`movswl %ax,%ebx` + `cmp %eax,%ebx; jge`）。
        // 交换比较两侧即复现 ORIG 的装载序与 fold 方向。
        if ((m_playMapNo >= 0) &&
            (G_CDataManager()->m_mapList->GetPVPMapCount() > (int)m_playMapNo))
        {
            GetInstanceCommonStatisticsMgr()->IncreaseQuantity((CCommonStatisticsMgr::ENUM_STATISTIC_CONTAINER)2, (int)m_playMapNo);
        }
        else
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x942, 0)(
                "START_PVP filter select_map_index(%d)", (int)m_playMapNo);
        }
        ((IMatch*)m_matching)->OnStartFight();
    }
LAB_085d91da:
    ;
}
