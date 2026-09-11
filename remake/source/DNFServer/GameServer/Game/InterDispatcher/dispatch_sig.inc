// ORIG 0x084beb3c T dispatch_sig —— 用户断开清理：状态门/扩展槽切换/付费税上报/各管理器离开通知/DB 收尾。
// 报告 docs/class_func_reports/Inter_Disconnect/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=176（ORIG 0x084beb3c）：寄存器中转(N18a/N7)+混合残差；gap-4。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=52（ORIG 0x84beb3c）：N18a 单段 regmove+N82/P112 帧槽平移。
// [lane2 2026-09-10] verdict=DIFF→AE：真 bug 修复 —— ① private_store vtable 槽错位（旧 `->Enter(user)`
//   走槽+1=0x85caa8a；ORIG 084beb3c 段内为槽+0=`CPrivateStoreMgr::Exit` 0x85ca994，nm T 直证）改 `->Exit(user)`；
//   ② SendMemberPayTax 后两实参（is_connect_upper_member/getCurCharacNo）由具名局部改内联实参
//   （ORIG %esi/%ebx 跨 GetServerGroup/GetServerProxy 保活，P52/P200）；③ assaultMgr 去具名局部。
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
}