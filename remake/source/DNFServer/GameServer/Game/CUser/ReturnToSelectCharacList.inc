void CUser::ReturnToSelectCharacList(bool param)
{
    UpdateCurCharacVisibleToCharacViewMemory();
    WorkPerFiveMin();

    int memberKey = get_charac_memberkey();
    if (memberKey != 0)
    {
        int payMoney = get_member_pay_tex_money_to_upper();
        int payFatigue = get_member_pay_tex_fatigue_to_upper();
        unsigned int characNo = getCurCharacNo();
        bool isUpper = is_connect_upper_member();
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup());
        proxy->SendMemberPayTax(isUpper, characNo, payMoney, payFatigue);
        if (payMoney > 0)
            set_member_pay_tex_money_to_upper(0);
        if (payFatigue > 0)
            set_member_pay_tex_fatigue_to_upper(0);
    }

    CGameManager* gm = G_CGameManager();
    set_uppermember_exp_level(0);
    set_connect_upper_member(0);
    gm->CheckOutTrade(this);
    gm->CheckOutParty(this, false);
    gm->CheckOutPvp(this, false);
    gm->CheckOutWarRoom(this);
    gm->checkOutBlueMarble(this);

    private_store::CPrivateStoreMgr* psMgr = private_store::GetInstancePrivateStoreMgr();
    (*(void(**)(private_store::CPrivateStoreMgr*, CUser*))(*(void**)psMgr))(psMgr, this);

    	pvp_assault::CAssaultMgr* assaultMgr = pvp_assault::GetInstanceAssaultMgr();
	assaultMgr->OnLeaveAssaultPlace(this, true);
	GlobalData::s_ExpertJobMgr->OnLeaveUser(this);
	long playTick = GetCharacPlayTick();
	int characNo = get_charac_no(-1);
	DB_UpdateCharacPlayTime::makeRequest(characNo, playTick);
	GlobalData::s_onlinePreliminaryTeamMgr->OnLeaveUser(this);
	unsigned int curCharacNo = getCurCharacNo();
	unsigned char villageCnt = GetCurVillageAttackCount();
	unsigned char revengeCnt = GetCurRevengeDungeonCount();
	if (IsVillageAttackDBUpdate())
		DB_UpdateRevengeData::makeRequest(curCharacNo, villageCnt, revengeCnt);

    DBUpdateDBLogItem();
    ResetDBLogItem();
    UpdateData();
    LogoutCachedCharacter(1);

    GameWorld* gw = G_GameWorld();
    gw->leave_game_world(this);
    ResetCharacExpandData();

    {
        ARAD::Arad_DataManager* adm = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
        // 2026-09-10（FL2CUser 类型轴，同 DisConnSig）：ORIG 0x08686fee 段 p167 差集
        //   只有「我们多 1×`SsC2ERKSs` 拷贝构造 + 2×`SsD2Ev` + 1×`_Unwind_Resume`」，
        //   而形参 `findGameScriptESs` 为按值 std::string ⇒ 原码直传字面量
        //   （临时 elide 直达形参），无具名 name 局部（EH 清理块亦随之消失）。
        ARAD::Arad_EventPeriodDataManager* epdm =
            (ARAD::Arad_EventPeriodDataManager*)adm->findGameScript(
                "Arad_EventPeriodDataManager");
        epdm->delUserRewardData(this);
    }

    ResetCurCharac();
    resetSaveFlags();
    set_state((ch_state)2);
    doLinkCharacDisconnect();

    GameWorld* gw2 = G_GameWorld();
    gw2->InsertLoginUser(this);
    m_inventoryMemory.reset();
    m_quest.reset();

    long now = (long)time(0);
    getPad()->setCheckTime(now);
    	CerashopAddRestrict::Manager::ClearBuyRestrictItem(this);
	WongWork::CHandlePremium::handleReturn2CharacView(this);

    if (param)
    {
        PacketGuard pg;
        pg.put_header(1, 7);
        pg.put_byte(1);
        pg.finalize(true);
        Send(pg);
    }
