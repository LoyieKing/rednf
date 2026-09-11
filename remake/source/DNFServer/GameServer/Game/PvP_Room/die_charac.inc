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
// [L11 已 identical 2026-09-11] 本轮三修（237/237, gap=0, ndiff=0）：
//  ① **未使用的 `int mode;` 声明占 4B 具名槽**——删后全函数槽集与 ORIG 逐一对齐
//     （旧形 ORIG-only={-0x3c,-0x48,-0x57,-0x5c} vs ours={-0x40,-0x4c,-0x5b,-0x6c}，
//     帧 0x8c vs 0x7c）⇒ 范式 P-L11b：**无用具名标量声明会整体平移槽基**，-O0 下
//     该声明照样占槽（GCC 4.4 不 DCE）。
//  ② 日志 4 实参全内联（同 leave_room/set_seat_state，消具名 -0x1c 槽 + 对象深槽）。
//  ③ check_winner 尾两分支：ORIG 0x085d961b..0x085d96b9 `test %al,%al; je 0x085d966d`
//     ⇒ then 体 = al!=0（OnDiePlayer(...,0)+pvp_request_pvp_rank+return -1，用 -0x20）、
//     je 岛 = al==0（OnDiePlayer(...,m_pvpBattleMode==3)+return 0，用 -0x1c）。
//     旧形「条件 !=0 + then===3 体」条件与体错配（真语义 bug）；`==0` 形多一条 `xor $1`。
//  ④ GetCharacExpandData 结果槽 **必须是指针类型局部** `CMissionList_Charac*`：int 形
//     令 4.4 先装载 this（`mov -0x24(%ebp),%eax` 落在实参装载之前），指针形复现 ORIG
//     的「lea/mov 0x8 → mov 0x4 → mov (%esp)」右到左装载序（末 8 行差异）。
//     证据：ORIG 0x085d9605 `mov -0x24(%ebp),%eax; mov %eax,(%esp)` 在 0x085d95f7
//     `lea -0x57(%ebp),%eax; mov %eax,0x8(%esp)` **之后**。
int PvP_Room::die_charac(CUser* victim, CUser* killer)
{
    int local_30;
    int local_2c;
    CMissionList_Charac* local_28;
    int local_24;
    int local_20;
    // [L11 已证实 2026-09-11] 同 leave_room/set_seat_state：ORIG 0x085d9386 的
    // mode 槽 -0x5c 是**实参求值的匿名溢出槽**（逆序：mode 溢出；index/name/acc
    // 落 callee-saved edi/esi/ebx），非具名局部。旧 register 攒值形令 4.4 给具名
    // 槽 -0x1c + 对象深槽 -0x44；全内联实参形即复现 ORIG 分区。
    {
        CSwitchLog(__PRETTY_FUNCTION__, 0x9a1, 0, 0)(   // ORIG 同文（P139）
            "pvp@log %s,%s,%d,%d", victim->get_acc_name(),
            victim->getCurCharacName(), get_index(), get_pvp_battle_mode());
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
        local_28 = (CMissionList_Charac*)killer->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
        MissionClearCondition_Parameter local_5b((PVP_BATTLE_MODE)m_pvpBattleMode);
        local_28->Update_Kill_event(*killer, local_5b);
    }
    // [b28d-solo-PvP_Room] 真语义 bug 修正（分支体整体反置）：ORIG 0x085d961b
    // `call check_winner; test %al,%al; je 0x085d966d` —— al==0 落 fall-through
    // （OnDiePlayer(...,0)+pvp_request_pvp_rank+return -1），al!=0 落 0x085d966d
    // （OnDiePlayer(...,m_pvpBattleMode==3)+return 0）。
    // [L11 2026-09-11] 逐条核 ORIG 0x085d9616..0x085d96b9：`test %al,%al;
    // je 0x085d966d` ⇒ **then 体 = al!=0 的「传 false + pvp_request_pvp_rank +
    // return -1」**（fall-through，用 -0x20），**je 岛 = al==0 的「传
    // m_pvpBattleMode==3 + return 0」**（用 -0x1c）。旧形把 ==3 体写在 `!=0` 的
    // then 里（条件与体错配 = 真语义 bug）；`==0` 形则多一条 `xor $1` 取反物化。
    if (check_winner() != 0)
    {
        local_24 = get_user_seat(killer);
        m_relayMgr.OnDiePlayer(local_30, local_24, false);
        pvp_request_pvp_rank();
        return 0xffffffff;
    }
    local_20 = get_user_seat(killer);
    m_relayMgr.OnDiePlayer(local_30, local_20, m_pvpBattleMode == 3);
    return 0;
}