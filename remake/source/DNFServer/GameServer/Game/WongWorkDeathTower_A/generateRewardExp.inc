void CDeathTower::CPlayData::generateRewardExp(int memberIdx, int stage,
                                               unsigned int baseExp, CParty* party,
                                               CDungeonMgr& dungeonMgr)
{
    CUser* user = party->get_user(memberIdx);
    unsigned int reward = 0;

    const char limitFlag = dungeonMgr.getDungeon()->limitOfStackableItemInTower();
    if (limitFlag != '\0')
    {
        // ORIG 两分支重复代码：32 位乘积先物化、G_CDataManager 后取（求值序照 ORIG）
        reward = (unsigned int)(long long)((float)(long long)(stage * baseExp) *
            G_CDataManager()->m_deathTower.m_rewardExpWeight[stage]);
    }
    else
    {
        // ORIG 非有限堆叠档（+0x15f4）
        reward = (unsigned int)(long long)((float)(long long)(stage * baseExp) *
            G_CDataManager()->m_deathTower.m_illusionRewardExpWeight[stage]);
    }

    if (1100000 < reward)
    {
        const float logWeight =
            G_CDataManager()->m_deathTower.m_rewardExpWeight[stage];
        LogManager::logFormat(1, kFile_DeathTower, kFunc_generateRewardExp, 0x2ff,
            "BLOOD_LOG : EROOR DEATHTOWER REWARD EXP(%u), LAST STAGE(%d), BASE EXP(%u), EXP WEIGHT(%f)\n",
            reward, stage, baseExp, (double)logWeight);
    }

    stClearRewardExpParameter_t clearParam;   // 栈构造（ORIG ctor 0x0814a8b8）
    stClearRewardBonusExp_t bonus;            // 栈构造（ORIG ctor 0x0814a800）

    const float avatarBonus = (float)party->getAvatarBonus();
    const float creatureBonus = (float)party->getCreatureBonus(user);

    float pcRoomRate = 0.0f;
    {   // 事件 8：PC 房间燃烧（整除 /100 —— imul 0x51eb851f 形态实证）
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(8);
        if (ev->IsEventing(user))
            pcRoomRate = (float)(((CPCRoomBurningEvent*)
                GlobalData::s_event_manager->GetRepeatEvent(8))->GetExpFactor() / 100);
    }

    float expDoubleRate = 0.0f;
    {   // 事件 3：双倍经验（浮点 /100.0f，常量 _DAT_08c640b8 = 100.0f）
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(3);
        if (ev->IsEventing(0))
            expDoubleRate = (float)((CExpDoubleEvent*)
                GlobalData::s_event_manager->GetRepeatEvent(3))->GetExpFactor() / 100.0f;
    }

    int burningBonusExp = 0;
    {   // 事件 0xf：疲劳燃烧加成（千分率原始值）
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0xf);
        if (ev->IsEventing(0))
            burningBonusExp =
                ((CBurningFatigueEvent*)ev)->getBonusExpRate(user->getCurCharacUsedFatigue());
    }

    // 频道脚本 bonus rate（+0x378 = Server_Envir 组别，参照 CBossPlay 先例走
    // get_server_group() 访问器替代裸偏移）
    channel_script_t* channelScript =
        (channel_script_t*)G_CDataManager()->GetChannelScript();
    CEnvironment* env = G_CEnvironment();
    // [P0] ORIG 084627xx：channelNo/dungeonIdx **无具名局部**（值经 %ebx/%esi
    //   跨 call 保活直传 getBonusRate 的第 2/3 实参），且 G_CEnvironment 全函数
    //   只调一次（旧重建两处调用 + 两个具名槽 ⇒ 帧 0x178 vs ORIG 0x130）。
    const float channelRate =
        channelScript->getBonusRate(
            (unsigned char)env->m_serverEnvir.m_serverGroup,
            (unsigned int)env->get_channel_no(),
            (unsigned int)dungeonMgr.getDungeon()->get_index());

    float premiumAdvantageRate = 0.0f;
    if (user->IsHavePremiumAdvantage())
        premiumAdvantageRate =
            (float)user->GetPremiumInfo()->GetAdvantageExpRate() / 100.0f;

    const float serverEventExpRate = (float)CServerEvent::GetExpRate() / 100.0f;

    const float manageBonus = party->CalcurateManageBonus((ManageBonusType::T)0, user, reward);

    // 结算基础通关经验（exp 以引用进出）
    unsigned int expInOut = reward;
    reward = party->getClearRewardBonusExp(user, expInOut, clearParam, bonus);

    if (1100000 < reward)
    {
        LogManager::logFormat(1, kFile_DeathTower, kFunc_generateRewardExp, 0x324,
            "BLOOD_LOG : EROOR DEATHTOWER CLEAR REWARD EXP(%u)\n", reward);
        LogManager::logFormat(1, kFile_DeathTower, kFunc_generateRewardExp, 0x32e,
            "BLOOD_LOG : DEATHTOWER EXP PARAMETER : %f, %f, %f, %f, %f, %f, %f, %d\n",
            (double)avatarBonus, (double)creatureBonus, (double)pcRoomRate,
            (double)expDoubleRate, (double)channelRate,
            (double)premiumAdvantageRate, (double)serverEventExpRate,
            (int)burningBonusExp);
        reward = 1100000;
    }

    m_rewardExp[memberIdx] = (int)reward;   // ORIG *(uint*)(this+(idx+0x26c)*4+8)
