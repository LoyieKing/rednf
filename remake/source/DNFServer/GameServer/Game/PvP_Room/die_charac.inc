// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::die_charac（原 TU 第 3342 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-7 N82附] ORIG 0x085d9386：4 个 getter 结果先物化 %edi/%esi/%ebx 跨 cMyTrace 构造保活，4.4 改槽中转；P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] N82附: ORIG 0x085d9386 4 getter 结果物化 %edi/%esi/%ebx 跨 cMyTrace 构造保活
// [b28d-solo-PvP_Room] 形参序修复（真语义 bug）：ORIG 0x085d9386 用 param_1
// (0xc(%ebp)) 做 get_user_seat/P_WIN 检查/日志（= victim）、param_2 做
// IncreaseKillCount/Update_Kill_event（= killer）；调用点 ORIG 0x081ecf47
// `die_charac(dispatch_sig 的 user, target)` 第一实参即死者。原实现按
// (killer, victim) 声明 ⇒ 与 ORIG 位置相反（旧 DIFF 主因）。交换形参名后
// 函数体零改动、调用点零改动，机器码与 ORIG 对齐。
// [b28d-solo-PvP_Room] 本轮复测（P167 CLEAR 36/36）：日志段按 P87 register 形
// 预求值（get_pvp_battle_mode→槽、get_index→%edi、getCurCharacName→%esi、
// get_acc_name→%ebx，跨 CSwitchLog ctor 保活）后，0xc/0x10 形参错位与 ctor
// 前求值序全部归位；残余仅槽号（ORIG local_5c@-0x5c / CSwitchLog@-0x3c vs
// 4.4 -0x1c / -0x44）⇒ N82 槽分配策略封顶。
// [b28d-solo-PvP_Room] 本轮真语义 bug 修复：check_winner 调用点两分支体整体反置 ——
// ORIG 0x085d961b `call; test %al,%al; je 0x085d966d`：al==0 落 fall-through
// （OnDiePlayer(...,0)+pvp_request_pvp_rank+return -1），al!=0 落 0x085d966d
// （OnDiePlayer(...,m_pvpBattleMode==3)+return 0）⇒ `==0` 改 `!=0`（消 xor 物化 + 语义归位）。
int PvP_Room::die_charac(CUser* victim, CUser* killer)
{
    int mode;
    int local_30;
    int local_2c;
    int local_28;
    int local_24;
    int local_20;
    // [b28d-solo-PvP_Room] P87 register 形：ORIG 0x085d9386 在 CSwitchLog ctor
    // 之前按 get_pvp_battle_mode()→槽 -0x5c、get_index()→%edi、getCurCharacName()
    // →%esi、get_acc_name()→%ebx 顺序求值（跨 ctor 保活），varargs 按
    // acc/name/index/mode 压栈。
    int local_5c = get_pvp_battle_mode();
    register int local_58 = get_index();
    register const char* local_54 = victim->getCurCharacName();
    register const char* local_50 = victim->get_acc_name();
    {
        // [b28d-solo-PvP_Room] P227：ORIG 0x085d9386 的 CSwitchLog 为全表达式临时链
        // （ctor 前后槽/寄存器装载与 ORIG 同向）。
        CSwitchLog(__PRETTY_FUNCTION__, 0x9a1, 0, 0)(   // ORIG 同文（P139）
            "pvp@log %s,%s,%d,%d", local_50, local_54, local_58, local_5c);
    }
    if (m_state != 2)
    {
        return 0x13;
    }
    lock();
    local_30 = get_user_seat(victim);
    if (local_30 < 0)
    {
        unlock();
        return 0xffffffff;
    }
    if (P_WIN(local_30) != 1)
    {
        unlock();
        return 0x12;
    }
    P_WIN(local_30) = 0;
    unlock();
    PacketGuard local_4c;
    local_4c.put_header(0, 0x2e);
    local_4c.put_byte(local_30);
    // [b28d-t3-PoolBigThree] P59 块序：ORIG 0x085d95e? `test; je <L_m1>` 后
    // **fall-through 即 get_user_seat 分支**（-1 岛后置 @0x085d9605），旧 `==0`
    // 形把 -1 分支放在 fall-through ⇒ 整体与 ORIG 错位 8 行。改极性正向。
    // （本件台账 74→本轮起点 113（solo 形参序真修后布局重排）→**93**；残余 =
    // N82 槽号平移（-0x5c↔-0x1c 族）+ P75 装载时刻 + C2/C1 显示差。）
    if (killer != 0)
    {
        local_4c.put_byte(get_user_seat(killer));
    }
    else
    {
        local_4c.put_byte(-1);
    }
    local_4c.finalize(true);
    send_to_pvp(local_4c);
    if ((m_pvpBattleMode == 4) || (m_pvpBattleMode == 5))
    {
        local_2c = get_user_seat(killer);
        m_deathMatch.IncreaseKillCount(local_2c);
        m_deathMatch.IncreaseDeathCount(local_30);
        m_deathMatch.InsertTimerKilledUserForRevive(get_index(), local_30);
    }
    if (killer != 0)
    {
        local_28 = (int)killer->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
        MissionClearCondition_Parameter local_5b((PVP_BATTLE_MODE)m_pvpBattleMode);
        ((CMissionList_Charac*)local_28)->Update_Kill_event(*killer, local_5b);
    }
    // [b28d-solo-PvP_Room] 真语义 bug 修正（分支体整体反置）：ORIG 0x085d961b
    // `call check_winner; test %al,%al; je 0x085d966d` —— al==0 落 fall-through
    // （OnDiePlayer(...,0)+pvp_request_pvp_rank+return -1），al!=0 落 0x085d966d
    // （OnDiePlayer(...,m_pvpBattleMode==3)+return 0）。旧形 `==0` 把两分支体互换。
    if (check_winner() != 0)
    {
        local_20 = get_user_seat(killer);
        m_relayMgr.OnDiePlayer(local_30, local_20, m_pvpBattleMode == 3);
        return 0;
    }
    local_24 = get_user_seat(killer);
    m_relayMgr.OnDiePlayer(local_30, local_24, false);
    pvp_request_pvp_rank();
    return 0xffffffff;
}
