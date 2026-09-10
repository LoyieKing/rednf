void CUser::UpdateLogout(bool param)
{
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
        sig->m_saveMemberBonusFlag = (unsigned char)(IsEnableSaveMemberBonusFatigue() ? 1 : 0);
        DisableSaveMemberBonusFatigue();
        sig->m_memberBonusFatigue =
            (unsigned short)getAccountMemberBonusFatigue();
        resetAccountMemberBonusFatigue();
    }
    {
        // ORIG 0x8655791/0x8655803：0x71ba4 更新前后各打一条 trace。
        int mid = get_unique_id();
        int playTime = sig->m_playTime;
        cMyTrace tr1(__PRETTY_FUNCTION__, 0x22e0, 0);
        tr1("trace mid:%d playTime:%d login_time:%d now_time:%d",
            mid, playTime, m_premium.m_loginTime, (int)now);
    }
    m_premium.m_loginTime = now;
    {
        int mid = get_unique_id();
        int playTime = sig->m_playTime;
        cMyTrace tr2(__PRETTY_FUNCTION__, 0x22e2, 0);
        tr2("trace mid:%d playTime:%d login_time:%d now_time:%d",
            mid, playTime, m_premium.m_loginTime, (int)now);
    }

    // b28d-CUser p167：ORIG 0x8655869 `mov 0x704ac(%eax),%eax; test %eax,%eax`
    // 直读成员（非 get_acc_id 调用）。
    bool sendOut = m_accId != 0 ||
                   G_GameWorld()->IsIntegratedPvPBaseChannel() ||
                   G_GameWorld()->GetChannelType() == 7;
    if (sendOut)
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
                                (unsigned short)G_CEnvironment()->m_serverEnvir.m_channelNo,
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
