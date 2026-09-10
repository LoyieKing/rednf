bool CUser::gain_exp_sp(int exp, int& sp, int& sfp,
                        eExpAddReason reason, int param, bool flag)
{
    // 环境 work-mode==2（G_CEnvironment()+0x1a8 具名化见 CEnvironment.h:m_field1a8）
    // 时，49 级以上角色不入经验。
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 2)
    {
        if (get_charac_level() > 0x31)
        {
            exp = 0;
        }
    }
    if (exp < 0)
    {
        exp = 0;
    }
    // b28d-solo-W8_CU_G：ORIG 0x0866a4xx 对形参直读两次
    // `cmpl $1,0x18(%ebp)…; cmpl $2,0x18(%ebp)`（无槽往返，P0）⇒ 去 reasonVal
    // 具名局部，使用点内联 cast 表达式。
    if (static_cast<int>(reason) == 1 || static_cast<int>(reason) == 2)
    {
        m_character.m_f34 += exp;
    }
    m_character.m_f28 += exp;
    incPlayExpAdd(exp);

    bool levelUp = false;
    if (get_charac_level() < 0x55)
    {
        levelUp = _check_level_up(exp, sp, sfp, reason, param);
        // b28d-solo-W8_CU_G：ORIG 0x0866a5xx getCurCharacSkillR() 结果直接作 receiver
        //（无 skillR 具名局部/槽，P0 表达式直传）。
        int spBefore = reinterpret_cast<SkillSlot*>(
            const_cast<char*>(getCurCharacSkillR()))->
            get_remain_sp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(0));
        gain_sp(sp);
        int sfpBefore = reinterpret_cast<SkillSlot*>(
            const_cast<char*>(getCurCharacSkillR()))->
            get_remain_sfp_at_index(static_cast<ENUM_SKILL_TREE_KIND>(2));
        gain_sfp(sfp);
        history_log_sp(spBefore, sp, eSPAddReason_0);
        history_log_sfp(sfpBefore, sfp, eSPAddReason_0);
    }

    PacketGuard questPacket;
    if (levelUp)
    {
        CalcurateUserMaxLevel();

        // 当前等级追平最高装备等级时清除该标记。
        if (GetCurCharacMaxEquipLevel() != 0 &&
            get_charac_level() == GetCurCharacMaxEquipLevel())
        {
            SetCurCharacMaxEquipLevel(0);
        }

        getCurCharacQuestW()->ResetUrgentQuestWaitingList();
        // b28d-W8: P0/P183 —— ORIG 0x0866a5xx getCurCharacQuestR 结果 %eax 直传
        // get_quest_info receiver（ORIG 无 questR 具名槽，我方原码多 store+load 2 条）。
        getCurCharacQuestR()->get_quest_info(reinterpret_cast<char*>(&questPacket));
        m_characterView.enableSaveCharacView();
        Send(questPacket);

        if (get_charac_level() == 0x55)
        {
            resetCharacFatigueGrownUpBuff();
        }

        _UpdateUserInfo4Guild();

        {
            // b28d-W8: ORIG 0x0866a67d `call; movsbl %al,%eax` ⇒ uchar→char 有符号
            // 转换（直接 static_cast<short>(uchar) 产 movzbl）；具名局部形实测
            // 396 条（+6）反劣，维持内联。
            GlobalData::s_guild_proxy_mgr->GetServerProxy(
                static_cast<ENUM_SERVER_GROUP>(GetServerGroup()))->
                SendCharLevelGrowType(get_acc_id(),
                                      static_cast<unsigned int>(get_charac_no(-1)),
                                      static_cast<short>(get_charac_level()),
                                      static_cast<short>(static_cast<char>(
                                          getCurCharacGrowType())));
        }
        {
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                static_cast<ENUM_SERVER_GROUP>(GetServerGroup()))->
                SendCharLevelGrowType(get_acc_id(),
                                      static_cast<unsigned int>(get_charac_no(-1)),
                                      static_cast<short>(get_charac_level()),
                                      static_cast<short>(static_cast<char>(
                                          getCurCharacGrowType())));
        }

        if (!VerifyPresentAvengerTitle())
        {
            // b28d-W8: P227 全表达式临时链 —— ORIG 0x0866a77b..0x0866a7e0 在 ctor
            // 前右起求值 characNo(%esi)→NumberToString(%ebx)，无具名槽（P0）。
            cMyTrace(__PRETTY_FUNCTION__, 0x5046, 0)(
                "CUser::VerifyPresentAvengerTitle() m_id(%s), charac_no(%u)",
                NumberToString(get_acc_id(), 0), getCurCharacNo());
        }

        AddCurCharacMercenaryInfo();
    }

    // 疲劳奖励 channel 组（5/8/7/10/12）。
    if (m_chState == 5 || m_chState == 8 || m_chState == 7 ||
        m_chState == 10 || m_chState == 12)
    {
        if (!flag)
        {
            SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                           static_cast<ENUM_NOTIPACKET>(0x25), 0);
            set_charac_fatigue_buf_bonus_exp(0);
        }
        else if (levelUp)
        {
            SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                           static_cast<ENUM_NOTIPACKET>(0x25), 0);
        }
        if (levelUp)
        {
            decide_growth_power_reward_system();
        }
    }

    if (levelUp)
    {
        // b28d-W8b: P329/P183 —— ORIG 0x0866a8a0 段无具名槽：playCount 先算留 %ebx
        // 跨 call、level 后算留 %eax、receiver s_statisticMgr 载 %edx（0x0866a8c1
        // `mov 0x941f764,%edx`）；expertExp 亦直压 0x4(%esp)（0x0866a8f0）。
        // 具名局部形多 3 槽 + 帧 0x10（389/388）⇒ 内联回实参位。
        GlobalData::s_statisticMgr->m_levelDungeonPlay.IncreaseLevelDungeonPlay(
            get_charac_level(), GetCurCharacDungeonPlayCount());
        ResetCurCharacDungeonPlayCount();

        expert_job::CExpertJob::IncreaseExpertJobExp(this,
                                                     GetCurCharacExpertJobExp());

        ReCalcChattingEmoticon();
        SendChattingEmoticon();
    }

    // b28d-W8b: P184/P171 —— ORIG 0x0866a96c 有**第二个独立** `if (levelUp)`
    //（`cmpb $0x0,-0x29; je +0x56e`，与检查 A 的 `je +0x522` 分离，4.1 不
    // jump-thread 合并）；块首两条死局部常量 0x0866a972/978 `movl $0x55,-0x20;
    // movl $0x50,-0x1c`（全程无读取，原码遗留照抄，P171）。
    if (levelUp)
    {
        int legacyMaxLevel = 0x55;
        int legacyMaxLevel2 = 0x50;
        (void)legacyMaxLevel;
        (void)legacyMaxLevel2;
        if (get_charac_level() == 0x55)
        {
            APSystem::CUserProc::ClearActionAndSendtoUser(
                this, APSystem::_ActionGroupIndex(0x21),
                0, static_cast<ENUM_PACKETCLASS>(0));
        }
    }
    return levelUp;
