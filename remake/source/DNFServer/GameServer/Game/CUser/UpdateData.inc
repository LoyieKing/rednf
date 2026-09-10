void CUser::UpdateData()
{
    if (m_chState <= 2)
        return;
    if (G_GameWorld()->GetChannelType() == 7)
        return;
    // ORIG +0x9f：无当前角色 → trace（0x1d99）后直接返回，不发任何保存。
    if (getCurCharacR() == 0)
    {
        // b28d-lane5 复测（P227）：ORIG 0x086532d0 段 getter 的 call 在 cMyTrace ctor
        //   **之前**、结果以 %ebx 跨 ctor 保活（`mov %eax,%ebx` … `mov %ebx,0x8(%esp)`）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1d99, 5)
            ("CUser::UpdateData() character %s - m_selected is null",
             getCurCharacName());
        return;
    }
    // ORIG +0x121：数据加载未完成 → trace（0x1da1）后直接返回。
    if (isLocked4DataLoad())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1da1, 5)
            ("Can not save character until loading done(m_id: %s, charac_no: %d",
             NumberToString(get_acc_id(), 0), getCurCharacNo());
        return;
    }

    if (getCurCharacInvenR()->checkToFix())
        getCurCharacInvenW()->CheckNFix();

    if (getCurCharacR() != 0 && get_charac_memberkey() != 0)
    {
        int payTaxMoney = get_member_pay_tex_money_to_upper();
        int payTaxFatigue = get_member_pay_tex_fatigue_to_upper();
        CMonitorServerProxy* monProxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup());
        if (monProxy != 0)
            monProxy->SendMemberPayTax(is_connect_upper_member() != 0,
                                       (unsigned int)getCurCharacNo(),
                                       payTaxMoney, payTaxFatigue);
        if (payTaxMoney > 0)
            set_member_pay_tex_money_to_upper(0);
        if (payTaxFatigue > 0)
            set_member_pay_tex_fatigue_to_upper(0);
    }

    _SaveCharacData();                    // ORIG T 0x08650aa2
    UpdateCharacView();
    ARAD::Singleton<HeroMissionEvent>::Get()->saveEventData(this, false);  // ORIG T 0x08166d38

    if (isSaveInven() || isSaveCargo())
    {
        // ORIG +0x27b：state(+0x8cfc4)==5（角色选择视口阶段）仅存背包。
        if (m_chState == 5)
            SaveInventoryOnly();
        else
            SaveInventory();
        disableSaveInven();
        disableSaveCargo();
    }

    WongWork::CMailBoxHelper::ReqDBSendStoredMail(this);

    if (isSaveSkill())
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1ded), true);
        guard.operator*()->operator<<((int)0x24);              // SIG_LOAD_SKILL
        guard.operator*()->operator<<((int)GetUID());
        SIG_LOAD_SKILL* sig = guard.operator->()->GetInBuffer<SIG_LOAD_SKILL>();
        memset(sig, 0, sizeof(SIG_LOAD_SKILL));           // 0x414
        // ORIG +0x394：+0x00 槽写 getCurCharacNo（movl 4B 实证）。
        sig->m_characNo = (unsigned int)getCurCharacNo();
        // ORIG 0x0865360e `movzbl 0xa850(%eax)`（unsigned char 宽实证）
        sig->m_field411 = G_CDataManager()->m_pvpSkillTree.m_field50;
        int sp1, sp2, sfp1, sfp2;
        ((SkillSlot*)getCurCharacSkillR())->get_remain_sp(&sp1, &sp2, &sfp1, &sfp2);
        sig->m_remainSpTree1 = sp1;
        sig->m_remainSpTree2 = sp2;
        sig->m_remainSfpTree1 = sfp1;
        sig->m_remainSfpTree2 = sfp2;
        _Mastered_skill* buf0 =
            ((SkillSlot*)getCurCharacSkillR())->get_skillslot_buf((ENUM_SKILL_TREE_KIND)0);
        _Mastered_skill* buf1 =
            ((SkillSlot*)getCurCharacSkillR())->get_skillslot_buf((ENUM_SKILL_TREE_KIND)1);
        if (buf0 != 0 && buf1 != 0)
        {
            // ORIG 拷贝 0x198 字节（= 0xcb 槽 × 2B，越界 2 字节读忠实保留）。
            memcpy(sig->m_skillSlotTree1, buf0, 0x198);
            memcpy(sig->m_skillSlotTree2, buf1, 0x198);
        }
        // ORIG +0x49a：技能命令区（+0x348，200 字节）拷贝后清零自身区（+0x703db）。
        STSkillCommandData::STCommand* cmdBuf = get_skill_command_buf();
        if (cmdBuf != 0)
            memcpy(sig->m_commands, cmdBuf, 200);
        memset(m_skillCommands, 0, sizeof(m_skillCommands));   // 200 = 0xc8
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        updateComboSkill();                 // ORIG T 0x08692396
        disableSaveSkill();
    }

    if (isSavePvP())
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1e2c), true);
        guard.operator*()->operator<<((int)0x25);              // SIG_SAVE_PVP
        guard.operator*()->operator<<((int)GetUID());
        SIG_SAVE_PVP* sig = guard.operator->()->GetInBuffer<SIG_SAVE_PVP>();
        memset(sig, 0, sizeof(SIG_SAVE_PVP));      // 0x88
        sig->m_characNo = getCurCharacNo();
        // ORIG：PvpResultType::Set((PvpResultType*)sig, getPVPResultRefR()) 整体拷入前段。
        ((PvpResultType*)sig)->Set(*getPVPResultRefR());
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        disableSavePvP();
    }

    if (isSaveDungeonClear())
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1e4e), true);
        guard.operator*()->operator<<((int)0x27);              // SIG_SAVE_DUNGEON_CLEAR
        guard.operator*()->operator<<((int)GetUID());
        char* sig = (char*)guard.operator->()->GetInBuffer<SIG_SAVE_DUNGEON_CLEAR>();
        memset(sig, 0, 0x180c);
        *(unsigned int*)(sig + 0) = (unsigned int)getCurCharacNo();
        *(unsigned int*)(sig + 4) = get_acc_id();
        // ORIG +0x77b：CDungeonClear::toString(sig+0xc, 0x1800) 序列化；
        // 返回负值（shr 31）走 "Quest buffer overflow" 报错 trace。
        if (getDungeonClearRefR()->toString(sig + 0xc, 0x1800) < 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1e5e, 5)
                ("Quest buffer overflow (m_id: %s, charac_no: %d",
                  NumberToString(get_acc_id(), 0), getCurCharacNo());
        }
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        disableSaveDungeonClear();
    }

    if (isSaveBattleRecord())
        disableSaveBattleRecord();     // ORIG +0x870：战斗记录脏标记仅清除

    if (isSaveQuest())
    {
        CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x1e8c), true);
        guard.operator*()->operator<<((int)0x29);              // SIG_UPDATE_QUEST
        guard.operator*()->operator<<((int)GetUID());
        SIG_UPDATE_QUEST* sig = guard.operator->()->GetInBuffer<SIG_UPDATE_QUEST>();
        memset(sig, 0, sizeof(SIG_UPDATE_QUEST));
        // questR+0x04 即 UserQuest::m_cleared（CQuestClear；+0x7538=m_quest[i]、
        // +0x7588=m_trigger[i]、+0x75f0/0x75f4 在 m_authenData 内）。
        // ORIG 每处重调 getCurCharacQuestR()（P44，含循环内），共 6 站点。
        if (getCurCharacQuestR()->m_cleared.checkSaveFlag())
        {
            memcpy(sig->m_clearedQuest,
                   getCurCharacQuestR()->m_cleared.getClearedQuest(), 30000);
            sig->m_clearedFlag = 1;
        }
        sig->m_characNo = (unsigned int)getCurCharacNo();
        for (int i = 0; i < 0x14; ++i)
        {
            sig->m_quest[i] =
                (unsigned int)getCurCharacQuestR()->m_quest[i];
            sig->m_trigger[i] =
                (unsigned int)getCurCharacQuestR()->m_trigger[i];
        }
        sig->m_urgentQuest =
            (unsigned int)getCurCharacQuestR()->m_authenData.m_urgentQuest;
        memcpy(sig->m_authen, getCurCharacQuestR()->m_authenData.m_authen, 0x28);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        // ORIG +0xa70：put 后清 QuestClear 保存标记再 disable。
        getCurCharacQuestW()->m_cleared.resetSaveFlags();
        disableSaveQuest();
    }

    // ORIG +0xaae：宠物（creature）项脏块经背包 CreatureMgr 落盘（读引用+R 视图取 W mgr，
    // 同 CUser.cpp 830 行先例 cast 掉 const）。
    if (((CInventory*)getCurCharacInvenR())->GetCreatureMgrW()->IsSaveCreatureItem())
        ((CInventory*)getCurCharacInvenR())->GetCreatureMgrW()->SaveCreatureItem();

    // ORIG 判定与取值各重调一次 GetCurCharacEventItemCount()（P44）。
    if (GetCurCharacEventItemCount() != 0)
    {
        // ORIG +0xafd：DB_UpdateItemCountValue::makeRequest(get_acc_id(), count)（SIG 0x240）。
        DB_UpdateItemCountValue::makeRequest(get_acc_id(),
                                             (unsigned int)GetCurCharacEventItemCount());
        ResetCurCharacEventItemCount();
    }

    if (isSaveInformNoticeFlag())
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1f2b), true);
        guard.operator*()->operator<<((int)0x2f5);             // InformNoticeFlagData
        guard.operator*()->operator<<((int)-1);
        char* sig = (char*)guard.operator->()->GetInBuffer<InformNoticeFlagData>();
        memset(sig, 0, 0x14);
        *(unsigned int*)(sig + 0) = get_acc_id();
        // ORIG +0xc05：i∈[0,0x10) 逐位 getInformNoticeFlag(i) 填 +4..+0x13。
        for (int i = 0; i < 0x10; ++i)
            *(sig + 4 + i) = (char)getInformNoticeFlag(i);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        disableSaveInformNoticeFlag();
    }

    if (GetCurCharacUsedFatigueQuantity() != 0)
    {
        // ORIG +0xc90：DB_UpdateUserFatigueQuantity::makeRequest(accId, qty)（SIG 0x287）。
        DB_UpdateUserFatigueQuantity::makeRequest(
            get_acc_id(),
            (unsigned int)GetCurCharacUsedFatigueQuantity());
        ResetCurCharacUsedFatigueQuantity();
    }
    // ORIG +0xcd8：赠礼疲劳脏标记 → 三参 DB 请求（SIG 0x288，accId/characNo/giftQty
    // 依报告反编译 C 实参序）后复位标记。
    if (GetChangedGiftFatigueQuantity())
    {
        DB_UpdateUserGiftFatigueQuantity::makeRequest(
            get_acc_id(), getCurCharacNo(),
            (unsigned int)GetCurCharacUsedGiftFatigueQuantity());
        SetChangedGiftFatigueQuantity(false);
    }

    // ORIG +0xd24：事件 0x19b（OBJECT_BRING_UP）进行中且攒有材料数 → 投递 SIG 并复位。
    EventClassify::CEventScriptMng* esm =
        (EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng();
    if (esm->is_eventing(0x19b) && getCurUseBringUpMaterialCount() != 0)
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1f73), true);
        guard.operator*()->operator<<((int)0x2ca);             // SIG_OBJECT_BRING_UP
        guard.operator*()->operator<<((int)-1);
        SIG_OBJECT_BRING_UP* sig = guard.operator->()->GetInBuffer<SIG_OBJECT_BRING_UP>();
        sig->m_accId = get_acc_id();
        sig->m_characNo = getCurCharacNo();
        // ORIG：+0x08 = *(G_CEnvironment()+0x378) & 0xff = m_serverEnvir.m_serverGroup
        // 低字节。P167：ORIG 为内联字段直读（无 get_server_group() 调用），
        // 故用 m_serverEnvir.m_serverGroup 内联访问，勿经 get_server_group() 访问器。
        sig->m_serverGroup = (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
        sig->m_usedMaterialCnt = getCurUseBringUpMaterialCount();
        sig->m_rewardFlag = 0;           // ORIG：+0x0d 恒 0
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        resetCurUseBringUpMaterialCount();
    }

    // ORIG 判定与取值各重调一次 GetCurCharacEventAddItemCount()（P44）。
    if (GetCurCharacEventAddItemCount() != 0)
    {
        // ORIG +0xedc：DB_UpdateAddItemCountValue::makeRequest(accId, count)（SIG 0x280）。
        DB_UpdateAddItemCountValue::makeRequest(get_acc_id(),
                                                (unsigned int)GetCurCharacEventAddItemCount());
        ResetCurCharacEventAddItemCount();
    }

    // ORIG +0xee8：hack 类型区间 [0x65,0xfa0] 服务器计数落库并复位。
    for (int hackType = 0x65; hackType <= 0xfa0; ++hackType)
    {
        // ORIG 0x086541dd：守卫读一次 getServerHackCnt（iVar25 != 0），进入后
        // 再读第二次作为 ReqDBSaveHackInfo 实参（3 次 getHackAnalyzer / 2 次
        // getServerHackCnt，P167 缺口），勿缓存单次计数。
        if (getHackAnalyzer()->getServerHackCnt(
                (WongWork::ENUM_HACKTYPE)hackType) != 0)
        {
            WongWork::CHackAnalyzer::ReqDBSaveHackInfo(
                get_acc_id(), (unsigned int)get_charac_no(-1),
                (unsigned short)hackType, GetETC((short)hackType),
                getHackAnalyzer()->getServerHackCnt(
                    (WongWork::ENUM_HACKTYPE)hackType));
            getHackAnalyzer()->resetServerHackCnt(
                (WongWork::ENUM_HACKTYPE)hackType);
        }
    }
    // ORIG +0xfd6：MC 点>499 → 曝光计数与点落库（type 0x329）后清曝光。
    if (m_mcap.getPoint() > 499)
    {
        WongWork::CHackAnalyzer::ReqDBSaveHackInfo(
            get_acc_id(), (unsigned int)get_charac_no(-1), 0x329,
            (int)m_mcap.getPoint(), (int)m_mcap.getExposedCount());
        m_mcap.resetExposedCount();
    }

    // ORIG +0x1046：学校点（脏累加）落库后复位。
    if (getCurCharacSchoolPoint() != 0)
    {
        DB_UpdateSchoolPoint::makeRequest(get_acc_id(),
                                          getCurCharacSchoolPoint(),
                                          (int)GetUID());
        resetCurCharacSchoolPoint();
    }

    // ORIG +0x109a：血之最高纪录/维度进出/断线运气/断线副本计数等脏块逐一落库。
    if (isBloodBestRecordUpdate())
        UpdateBloodRecord();
    if (get_charac_level() > 0x3b &&
        (IsSaveDemensionInoutValue() || IsSaveBloodInoutValue()))
        UpdateDimensionInout();
    if (isUsedBreakAwayLuckPoint())
        UpdateBreakAwayLuckPoint();
    if (isBreakAwayDungeonClear())
        UpdateBreakAwayDungeonClearCnt();
    // ORIG +0x115b：角色链接奖励 exp/gold 任一带值 → 提交（state=2）。
    if (getCharacLinkBonusExp() != 0 || getCharacLinkBonusGold() != 0)
        UpdateCharacLinkBonus(getCharacLinkBonusExp(), getCharacLinkBonusGold(),
                              (unsigned int)getLinkCharacNo(getLinkCharacSlotIndex()),
                              (ENUM_CHARAC_LINK_BONUS_STATE)2);

    // ORIG +0x11c3：NPC 关系 / 扩展数据链 / 条件事件 / 适正副本数。
    _updateNPCRelationship();
    m_characExpand.update(this);
    _saveConditionEventInfo(getCurCharacNo(), GetCurConditionEventStep(),
                            GetCurConditionEventRewardStep());
    _saveProperDungeonClearCount(getCurCharacNo(),
                                 GetProperDungeonClearCount());
    if (isSaveCharacOption())
        _updateSaveOption();

    // ORIG +0x1275：账号仓库有改动 → DB_SaveAccountCargo（SIG 0x1f1；随后 SetStable）。
    if (IsExistAccountCargo())
    {
        CAccountCargo* cargo = GetAccountCargo();
        if (cargo->IsAlter())
        {
            DB_SaveAccountCargo::makeRequest((int)GetUID(), get_acc_id(),
                                             cargo);
        }
    }
    // ORIG +0x12c7：金币保护存库 + 杂项周期落库。
    GetGoldControl()->SavetoDB(true, true, false);
    SaveMercenaryData();
    updatePcRoomPlayTime();
    updateBlueMarbleEnterCount();
    updateBingoData();
