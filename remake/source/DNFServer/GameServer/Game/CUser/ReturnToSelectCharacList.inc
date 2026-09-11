// ============================================================================
// CUser::ReturnToSelectCharacList @ 0x08686fee（ReturnToSelectCharacList.md）
//   返回角色选择列表：成员税上报 → 各管理器 CheckOut → 私商/突袭/专家/在线
//   团队离场 → 播放时间落库 → 复仇数据 → DB 日志/更新 → 登出缓存 → 离开
//   世界 → 重置角色/任务 → ARAD 周期数据清理 → 角色视图复位。
// ============================================================================
// 以下依赖符号均为 ORIG 真实符号；类/实现由对应域批次提供：
//   DB_UpdateCharacPlayTime::makeRequest(int,long)（DB 域）
//   DB_UpdateRevengeData::makeRequest(uint,uchar,uchar)（DB 域）
//   expert_job::CExpertJobMgr::OnLeaveUser(CUser*)（专家批次）
//   online_preliminary::COnlinePreliminaryTeamMgr::OnLeaveUser(CUser*)（在线批次）
//   WongWork::CHandlePremium::handleReturn2CharacView(CUser*)（premium 批次）
//   ARAD::Singleton<Arad_DataManager>::Get()（ARAD 域；SPECIAL）
// ============================================================================
// 注：以下依赖符号（DB_UpdateCharacPlayTime/DB_UpdateRevengeData、
// expert_job::CExpertJobMgr::OnLeaveUser、online_preliminary::
// COnlinePreliminaryTeamMgr::OnLeaveUser、WongWork::CHandlePremium::
// handleReturn2CharacView）均为 ORIG 真实符号，但对应权威类声明缺方法 /
// DB 请求类未建模，属外域缺口（BLOCKED，见报告）；其调用点在下方保留为
// 真实调用，由对应域批次补权威声明后即可编译。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08686fee | _ZN5CUser24ReturnToSelectCharacListEb
void CUser::ReturnToSelectCharacList(bool param)
{
    UpdateCurCharacVisibleToCharacViewMemory();
    WorkPerFiveMin();

    if (get_charac_memberkey())
    {
        int payMoney = get_member_pay_tex_money_to_upper();
        int payFatigue = get_member_pay_tex_fatigue_to_upper();
        unsigned int characNo = getCurCharacNo();
        // b28d-GrpD（P183 去具名 receiver 缓存）：ORIG 0x08687079 `call GetServerProxy`
        //   返回 eax 后**无槽写**，0x08687097 直接 call SendMemberPayTax；我方具名
        //   proxy 多 `mov %eax,-0xc(%ebp)` + `mov -0xc(%ebp),%eax` 各 1 条
        //   （探针 /tmp/L9/D/p5.cpp 形 C 直证 −2 条且 C1/C2 求值序不变）⇒ 内联 receiver。
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup())
            ->SendMemberPayTax(is_connect_upper_member(), characNo, payMoney,
                               payFatigue);
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

    	pvp_assault::GetInstanceAssaultMgr()->OnLeaveAssaultPlace(this, true);
	GlobalData::s_ExpertJobMgr->OnLeaveUser(this);
	DB_UpdateCharacPlayTime::makeRequest(get_charac_no(-1), GetCharacPlayTick());
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

    G_GameWorld()->leave_game_world(this);
    ResetCharacExpandData();

    {
        // 2026-09-10（FL2CUser 类型轴，同 DisConnSig）：ORIG 0x08686fee 段 p167 差集
        //   只有「我们多 1×`SsC2ERKSs` 拷贝构造 + 2×`SsD2Ev` + 1×`_Unwind_Resume`」，
        //   而形参 `findGameScriptESs` 为按值 std::string ⇒ 原码直传字面量
        //   （临时 elide 直达形参），无具名 name 局部（EH 清理块亦随之消失）。
        ((ARAD::Arad_EventPeriodDataManager*)ARAD::Singleton<ARAD::Arad_DataManager>::Get()
             ->findGameScript("Arad_EventPeriodDataManager"))
            ->delUserRewardData(this);
    }

    ResetCurCharac();
    resetSaveFlags();
    set_state((ch_state)2);
    doLinkCharacDisconnect();

    G_GameWorld()->InsertLoginUser(this);
    m_inventoryMemory.reset();
    m_quest.reset();

    getPad()->setCheckTime((long)time(0));
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
}