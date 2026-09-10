int Inter_Disconnect::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x651, "State(NONE)");
        return 0;
    }

    if (user->get_state() > 2)                         // cmp $2; setg
    {
        if (G_GameWorld()->IsEquipSlotSwitchChannel())
        {
            if (user->getCurChannelEquipslotSwitch() == '\x01')
            {
                CExpandEquipslot* expand =
                    (CExpandEquipslot*)user->GetCharacExpandData(
                        (ENUM_CHARAC_EXPAND_TYPE)9);
                expand->EquipslotSwitch(
                    user, '\0',
                    user->getCurChannelEquipslotSwitch(),
                    user->getCurExpandEquipslotSwitch());
            }
        }
    }

    user->WorkPerFiveMin();

    if (user->get_charac_memberkey() != 0)
    {
        const int payMoney = user->get_member_pay_tex_money_to_upper();
        const int payFatigue = user->get_member_pay_tex_fatigue_to_upper();
        // ORIG 084bf3xx：characNo→%ebx / isUpper→%esi 跨 GetServerGroup/GetServerProxy
        //   保活（无栈槽）⇒ 后两实参内联到调用点（P52/P200）；receiver 链不设具名局部。
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
            ->SendMemberPayTax(user->is_connect_upper_member() != 0,
                               user->getCurCharacNo(), payMoney, payFatigue);
        if (0 < payMoney)
            user->set_member_pay_tex_money_to_upper(0);
        if (0 < payFatigue)
            user->set_member_pay_tex_fatigue_to_upper(0);
    }

    // 真 bug 修正（P193/NM 实证）：ORIG 084bf4xx 调用 vtable 槽+0 = CPrivateStoreMgr::Exit
    //   （ORIG 0x85ca994 nm T `_ZN13private_store16CPrivateStoreMgr4ExitEP5CUser`）；
    //   旧写 Enter 走槽+1（0x85caa8a）多一条 `add $0x4,%edx`。
    private_store::GetInstancePrivateStoreMgr()->Exit(user);
    pvp_assault::GetInstanceAssaultMgr()->OnLeaveAssaultPlace(user, true);
    GlobalData::s_ExpertJobMgr->OnLeaveUser(user);              // expert_job::CExpertJobMgr（ORIG T 0x0849e214）
    GlobalData::s_onlinePreliminaryTeamMgr->OnLeaveUser(user);  // online_preliminary（ORIG T 0x08588ae8）

    const unsigned int accId = user->get_acc_id();
    const unsigned int uid = user->GetUID();
    if (user->m_mcap.IsSaveCleanPadPoint())
    {
        // [b23S3C] P48：ORIG 此处二次 call get_acc_id（uVar6 缓存供尾部三件，本点重新取）。
        DB_SaveCleanpadPoint::makeRequest(user->m_mcap.getPoint(), user->get_acc_id());
        user->m_mcap.disableCleanPadPoint();
    }
    G_CGameManager()->user_disconnect(user);
    DB_ArrangeUserLoginInfo::makeRequest(accId, true, uid);
    DB_SecurityCardUpdateValidityTime::makeRequest(accId, 0);
    DB_GoblinPadUpdateValidityTime::makeRequest(accId, 0);
    return 0;
