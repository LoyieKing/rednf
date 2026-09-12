// 0x086554aa：登出主链路（UpdateLogout.md）。SIG_UPDATE_LOGOUT(0x2c, 0x8b 字节)：
// accId/characNo/gcNo/fatigue；state>1 记 playTime/IP/机器码/会员奖励疲劳并复位；
// 两条 trace（0x22e0/0x22e2）；写登录时间戳；历史日志 Logout；DB 杂项（复仇/
// ontime/疲劳出勤）；!param 发包(1,3)；清商城内购限制。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086554aa | _ZN5CUser12UpdateLogoutEb
void CUser::UpdateLogout(bool param)
{
    // 2026-09-10 gA（A 组）负结论 N（ORIG 492 vs 我方 513）：ORIG 0x086554aa 与 p167
    //   已对齐的调用序一致（CStreamGuard::operator*/-＞、sig 具名槽、DB 段、两处
    //   cMyTrace），差集为**全帧槽号整体平移**（param -0xac→-0xbc、guard -0x50→-0x44、
    //   sig -0x28→-0x3c；帧 0xcc→0xdc）——属 N82/P112 槽分配族（4.1 与 4.4 局部
    //   分配方向/复用不同），逐块无字段或调用缺口可下刀 ⇒ N 封顶。
    // 2026-09-11 R5 补（disp2 命中终判，门②/③d）：disp2 报本件 ORIG-only `+0x47`。核指令
    //   上下文：ORIG 0x08655735 `lea 0x47(%eax),%edx`（eax = -0x28(%ebp) 槽值）→
    //   `movl $0x41,0x8(%esp); lea -0x9d(%ebp),%eax; …; call <memcpy>` ⇒ 是**局部指针槽的
    //   地址算术**（0x47 是目标缓冲偏移），**非结构成员读**；我方同点 `add $0x47,%eax`
    //   （imm 轴同值命中）⇒ 门② 同值互补 + 门③d ⇒ 等价，非 bug，勿再改。
    UpdateCharacView();

    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x2260), true);
    // b28d-CUser p167 真缺口：ORIG 0x086555e0/0x086555f1 显式调 CStreamGuard::operator*
    //   （deEv）×2、0x086555?? operator->（ptEv）×1 再 GetInBuffer（P132/P103）；
    //   直接 `guard <<` / `guard.GetInBuffer` 少 3 个调用点。
    (*guard)->operator<<((int)0x2c);
    (*guard)->operator<<((int)GetUID());
    SIG_UPDATE_LOGOUT* sig =
        guard.operator->()->GetInBuffer<SIG_UPDATE_LOGOUT>();
    memset(sig, 0, sizeof(SIG_UPDATE_LOGOUT));
    // b28d-CUser p167：ORIG 0x8655582 `mov 0x704ac(%eax),%eax` 直读成员
    // （非 get_acc_id 调用；该访问器在 ORIG 本函数仅出现 2 次，见下方 DB 段）。
    sig->m_accId = m_accId;
    // ORIG 0x865557c：IntegratedPvP 频道用 get_gc_no_hardcode(GetServerGroup())。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        sig->m_gcNo =
            (unsigned int)G_CEnvironment()->get_gc_no_hardcode(GetServerGroup());
    else
        sig->m_gcNo = (unsigned int)G_CEnvironment()->get_gc_no();
    if (getCurCharacR() != 0)
    {
        sig->m_characNo = (unsigned int)getCurCharacNo();
        sig->m_fatigue = GetFatigue();
    }
    ((XNuclear::CHades*)getHades())->Send_Logout();   // ORIG 0x8655605

    long now = (long)GlobalData::s_systemTime_.getCurSec();
    sig->m_stateFlag = 0;
    if (get_state() > 1)
    {
        sig->m_stateFlag = 1;
        sig->m_playTime =
            (int)(now - m_premium.m_loginTime);   // 推断：登录时刻
        sig->m_field17 = (unsigned int)m_tradeCount;   // CUser+0x8d10c（ORIG movl）
        sig->m_playExpAdd = m_gameOption.m_playExpAdd;   // ORIG this+0x703a4 = m_gameOption+0x640
        sig->m_totalFatigueUsed =
            (unsigned short)m_postChecksumData.m_totalFatigueUsed;   // ORIG this+0x79642 实证
        // b28d-CUser p167 真缺口：ORIG 0x865568e `movzbl 0x8d0df(%eax),%edx;
        //   mov %dl,0x21(%eax)` 直读 this+0x8d0df（= m_guildAgitDB.m_pad2[0]，
        //   SigTypes_E.h:521 注释同证）；原写 GetPuUser() 读的是 +0x796d8（错字段，
        //   且多发一次 call）。
        sig->m_puUser = (unsigned char)m_guildAgitDB.m_pad2[0];
        SetUserManagerToolIP(this, reinterpret_cast<SIG_UPDATE_LOGOUT*>(sig));  // ORIG 0x865569e 实证传结构基址

        // b28d-CUser p167：ORIG 0x86556ad 起为非对齐 peel + rep stos 内联清零
        // （无 memset call）⇒ 用 `= {0}` 常量清零形（P214③/P253）。
        char mac[0x41] = {0};
        memcpy(mac, m_network.get_mac_addr(), 0x41);
        strncpy(sig->m_mac, mac, 0x41);
        sig->m_saveMemberBonusFlag = IsEnableSaveMemberBonusFatigue();
        DisableSaveMemberBonusFatigue();
        sig->m_memberBonusFatigue =
            (unsigned short)getAccountMemberBonusFatigue();
        resetAccountMemberBonusFatigue();
    }
    {
        // ORIG 0x8655791/0x8655803：0x71ba4 更新前后各打一条 trace。
        // ORIG 0x08655791-0x086557cc：mid/playTime **无栈槽**——`movzwl %ax,%ebx`（mid 直入
        // callee-saved）、`mov -0x28(%ebp),%eax; mov 0x13(%eax),%esi`（sig->m_playTime 直读）。
        // 我方两具名 int 各占一槽 ×2 块 = 4 槽，正是帧 0xdc vs 0xcc（+0x10）的唯一来源。
        // [R16a04 P227] ORIG 0x08655796..0x086557d1：`get_unique_id`(%ebx) /
        //   `sig->m_playTime`(%esi) / 0x71ba4(%edi) 三个 vararg **在 cMyTrace::C1
        //   （0x086557cc）之前**求值并跨 ctor 保活 ⇒ 原码是构造即调用的临时链
        //   （具名对象形把 C1 提前到求值之前）。
        cMyTrace(__PRETTY_FUNCTION__, 0x22e0, 0)
            ("trace mid:%d playTime:%d login_time:%d now_time:%d",
             get_unique_id(), sig->m_playTime, m_premium.m_loginTime, (int)now);
    }
    m_premium.m_loginTime = now;
    {
        // [R16a04 P227] 同 tr1（ORIG 0x08655808..0x0865583e 于 C1 前求值保活）。
        cMyTrace(__PRETTY_FUNCTION__, 0x22e2, 0)
            ("trace mid:%d playTime:%d login_time:%d now_time:%d",
             get_unique_id(), sig->m_playTime, m_premium.m_loginTime, (int)now);
    }

    // b28d-CUser p167：ORIG 0x8655869 `mov 0x704ac(%eax),%eax; test %eax,%eax`
    // 直读成员（非 get_acc_id 调用）。
    // ORIG 0x08655869..0x086558a5：三段短路全部直判（`test %eax,%eax; jne` /
    // `test %al,%al; jne` / `cmp $0x7,%eax; jne` → `mov $1/$0` 合流），**无 bool 槽**；
    // 我方具名 sendOut 多 `mov %al,-0x31(%ebp); cmpb $0x0,-0x31(%ebp)`。
    if (m_accId != 0 || G_GameWorld()->IsIntegratedPvPBaseChannel() ||
        G_GameWorld()->GetChannelType() == 7)
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    else
        GlobalData::s_stream_pool->Free(guard);

    if (m_network.get_str_ip()[0] != '\0' && getCurCharacName() != 0)
    {
        // b28d-CUser p167 真缺口：ORIG 0x8655915 / 0x8655955 调用
        //   GetCharacStartPlayTick **两次**（条件一次、实参一次，P157/P48 逐使用点重调）；
        //   实参求值序（cdecl 右→左）：GetLogInOutState → getCurSec → tick2 → …
        if (GetCharacStartPlayTick() > 0)
        {
            m_historyLog.Logout(getWebAddress(), m_network.get_str_ip(),
                                m_network.get_port(),
                                // ORIG 0x08655969 `mov 0x1b0(%eax),%edi` 为 32 位直读
                                // （无 movzwl）⇒ 原码无截断 cast；第 4 参 int d 经
                                // LogHistory "%d" 完整使用（R3a04 第三轮 SEMANTIC-DIFF 修正）。
                                G_CEnvironment()->m_serverEnvir.m_channelNo,
                                (int)((long)GlobalData::s_systemTime_.getCurSec() -
                                      GetCharacStartPlayTick()),
                                GetLogInOutState());
        }
    }

    DBUpdateDBLogItem();
    ResetDBLogItem();

    {
        unsigned int characNo = getCurCharacNo();
        unsigned char villageCnt = GetCurVillageAttackCount();
        unsigned char revengeCnt = GetCurRevengeDungeonCount();
        if (IsVillageAttackDBUpdate())
            DB_UpdateRevengeData::makeRequest(characNo, villageCnt, revengeCnt);
    }
    if (is_update_ontime_last_recv_idx())
    {
        DB_UpdateOntimeLastRecvIdx::makeRequest(GetUID(), get_acc_id(),
                                                get_ontime_last_recv_idx());
        reset_update_ontime_last_recv_idx();
    }
    if (((EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng())
            ->is_eventing(0x56))
    {
        if (get_server_fatigue_day_size() != 0)
        {
            // b28d-CUser p167 真缺口：ORIG 0x8655ac9 `call get_server_fatigue_day_data;
            //   mov %eax,%ebx; … mov %ebx,0x4(%esp)` —— 取地址走 getter，非成员直传。
            DB_UpdateFatigueAttendance::makeRequest(
                get_acc_id(),
                *(const std::map<unsigned char, short>*)
                    get_server_fatigue_day_data());
            reset_server_fatigue_data();
        }
    }
    reset_ingame_event_history();

    if (!param)
    {
        PacketGuard pkt;
        ((InterfacePacketBuf*)&pkt)->put_header(1, 3);
        ((InterfacePacketBuf*)&pkt)->put_byte(1);
        ((InterfacePacketBuf*)&pkt)->finalize(true);
        Send(pkt);
    }
    CerashopAddRestrict::Manager::ClearBuyRestrictItem(this);
}