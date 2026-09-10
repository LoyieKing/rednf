bool CBossPlay::generateRewardExp(int memberIdx, unsigned int a, unsigned int b,
                                 const CDungeon* dungeon)
{
    CUser* user = m_party->get_user(memberIdx);
    if (user == 0)
        return 0;

    // 基础经验 = round((float)(a*b) × 死亡之塔费率)。GetTowerScript 返回结构体基址，
    // ORIG flds 0x4(%eax,%edx,4) 读 m_rewardExpWeight[1+a]（[reward exp weight] 表，
    // 脚本下标从 1 起）；x87 fistpll 就近舍入，直接 (long long) 强转等价。
    unsigned int clearExp;
    {
        // [b28d-46 P252 实测] ORIG 先 fildll(a*b)→fstps 具名 float 槽 -0xf0，再取
        // tower 并 fmuls mem；具名 float 局部复现该槽往返。
        float baseExp = (float)(long long)((unsigned int)(a * b));
        // ORIG tower 无槽往返（GetTowerScript 结果直用 %eax）⇒ 内联表达式（P0/P183）。
        clearExp = (unsigned int)(long long)(
            baseExp * G_CDataManager()->GetTowerScript(
                dungeon->get_index())->m_rewardExpWeight[1 + a]);
    }

    stClearRewardBonusExp_t bonus;
    stClearRewardExpParameter_t param;
    float avatarBonus = (float)m_party->getAvatarBonus();
    float creatureBonus = (float)m_party->getCreatureBonus(user);

    float serverRate = (float)CServerEvent::GetExpRate() / kPercentDivisor;

    CDataManager* dm2 = G_CDataManager();
    channel_script_t* chScript = (channel_script_t*)dm2->GetChannelScript();
    CEnvironment* env = G_CEnvironment();
    unsigned int channelNo = (unsigned int)env->get_channel_no();
    // 渠道组字节：ORIG 重取 G_CEnvironment() 并内联读 +0x378（= Server_Envir
    //             m_serverGroup，+0x198+0x1e0），不 call get_server_group()。
    unsigned char channelType =
        (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
    float channelRate =
        chScript->getBonusRate(channelType, channelNo, dungeon->get_index());

    float pcrBurning = 0.0f;
    float expDouble = 0.0f;
    // [L4-r3 判据⑫] widthscan: clr_l orig=0 / our=1 ⇒ ORIG 无任何 int 具名局部的
    // `=0` 初始化（我方多 1 条 `movl $0,-N`）⇒ 去初始化器（赋值点在使用前）。
    int burningRate;
    float premiumRate = 0.0f;

    CEventBase* ev8 = GlobalData::s_event_manager->GetRepeatEvent(8);
    if (ev8->IsEventing(user))
        // ORIG：整型 /100（imul 0x51eb851f 形态）后才转 float
        pcrBurning = (float)(((CPCRoomBurningEvent*)
            GlobalData::s_event_manager->GetRepeatEvent(8))->GetExpFactor() / 100);

    CEventBase* ev3 = GlobalData::s_event_manager->GetRepeatEvent(3);
    if (ev3->IsEventing(0))
        expDouble = (float)((CExpDoubleEvent*)
            GlobalData::s_event_manager->GetRepeatEvent(3))->GetExpFactor() / kPercentDivisor;

    CEventBase* evF = GlobalData::s_event_manager->GetRepeatEvent(0xf);
    if (evF->IsEventing(0))
        burningRate =
            ((CBurningFatigueEvent*)
                GlobalData::s_event_manager->GetRepeatEvent(0xf))->getBonusExpRate(user->getCurCharacUsedFatigue());

    if (user->IsHavePremiumAdvantage())
    {
        WongWork::CUserPremium* pre = user->GetPremiumInfo();
        premiumRate = (float)pre->GetAdvantageExpRate() / kPercentDivisor;
    }

    unsigned int total = m_party->getClearRewardBonusExp(user, clearExp, param, bonus);

    if (0x10c8e0 < total)
    {
        LogManager::logFormat(1, "localglobal/global_PlayUser.cpp",
            __PRETTY_FUNCTION__,
            0x159, "BOSS_LOG : EROOR BOSSTOWER CLEAR REWARD EXP(%u)\n", total);
        LogManager::logFormat(1, "localglobal/global_PlayUser.cpp",
            __PRETTY_FUNCTION__,
            0x163, "BOSS_LOG : BOSSTOWER EXP PARAMETER : %f, %f, %f, %f, %f, %f, %f, %d\n",
            (double)avatarBonus, (double)creatureBonus, (double)pcrBurning, (double)expDouble,
            (double)channelRate, (double)premiumRate, (double)serverRate, burningRate);
        total = 0x10c8e0;
    }
    m_rewardExp[memberIdx] = (int)total;
    return 1;
