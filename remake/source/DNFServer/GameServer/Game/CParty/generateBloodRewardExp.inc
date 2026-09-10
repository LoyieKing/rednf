void CParty::generateBloodRewardExp(int slot, int round, unsigned int baseExp,
                                    short inoutCnt)
{
    // b28d-49 判词：N 封顶（N82 帧/槽；P282 虚派发偏移差 52 已核）。P167 CLEAR。@0x85b7ea2
    CUser* user = get_user(slot);                       // -0x1c
    unsigned int exp = 0;                               // -0x20
    // ORIG generateBloodRewardExp.md 085b7edc `movzbl 0x89f(%eax)` =
    // CDungeon::m_bloodDungeonType（unsigned char 宽度修正：原 *(char*) movsbl）
    if (m_battleField.m_pDungeon->m_bloodDungeonType == 2)             // 最终轮
    {
        // ORIG 0x85b7ea2：round*baseExp 32 位无符号乘法（零扩展 64 位）→
        // fraw 先转 float（fildll/fstps），weight 后取（flds），
        // weight*fraw*10.0f 栈上相乘 → fistpll 截断（无 +0.5）。
        long long raw = (long long)(round * baseExp);
        float fraw = (float)raw;
        exp = (unsigned int)(long long)(
            G_CDataManager()->m_bloodDungeon.m_rate[round] * fraw * 10.0f);
    }
    else
    {
        long long raw = (long long)(round * baseExp);
        float fraw = (float)raw;
        // ORIG：weight*fraw 先算落栈（fstps），再乘 inoutWeight（fmuls）。
        float w1 = G_CDataManager()->m_bloodDungeon.m_rate[round] * fraw;
        exp = (unsigned int)(long long)(w1 *
            G_CDataManager()->m_bloodDungeon.m_vec334[(unsigned int)inoutCnt]);
    }
    if (1000000 < exp)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x3cec,
            "BLOOD_LOG : EROOR BLOOD CLEAR REWARD EXP(%u), BASE EXP (%u), "
            "EXP_WEIGHT(%f), EXP_INOUT_WEIGHT(%f), LAST_ROUND(%d), "
            "INOUT_CNT(%d)\n",
            exp, baseExp,
            G_CDataManager()->m_bloodDungeon.m_rate[round],
            G_CDataManager()->m_bloodDungeon.m_vec334[(unsigned int)inoutCnt],
            round, (int)inoutCnt);
    }

    stClearRewardBonusExp_t bonus;                      // -0x68
    stClearRewardExpParameter_t param;                  // -0xc8
    param.m_avatarBonus = getAvatarBonus();
    param.m_creatureBonus = getCreatureBonus(user);

    CEventBase* ev8 = GlobalData::s_event_manager->GetRepeatEvent(8);
    CPCRoomBurningEvent* pcr = static_cast<CPCRoomBurningEvent*>(ev8);
    if (pcr->IsEventing(user))
        param.m_pcRoomBurningRate = (float)(static_cast<CPCRoomBurningEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(8))->GetExpFactor() / 100);

    CEventBase* ev3 = GlobalData::s_event_manager->GetRepeatEvent(3);
    CExpDoubleEvent* expDouble = static_cast<CExpDoubleEvent*>(ev3);
    if (expDouble->IsEventing(0))
        param.m_expDoubleRate = (float)(static_cast<CExpDoubleEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(3))->GetExpFactor() / 100.0f);

    CBurningFatigueEvent* fatigue =
        static_cast<CBurningFatigueEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0xf));
    if (fatigue->IsEventing(0))
        param.m_burningFatigueBonusExp = fatigue->getBonusExpRate(
            reinterpret_cast<CUserCharacInfo*>(user)->getCurCharacUsedFatigue());

    channel_script_t* chScript =
        (channel_script_t*)G_CDataManager()->GetChannelScript();
    unsigned int dungIdx = m_battleField.m_pDungeon->get_index();
    CEnvironment* env = G_CEnvironment();
    unsigned int channelNo = (unsigned int)env->get_channel_no();
    // ORIG generateBloodRewardExp.md 085b824d `mov 0x378(%eax),%eax`（int dword
    // 读）+ Ghidra `(uchar)*(int*)(iVar4+0x378)` 截断。CEnvironment+0x378 =
    // m_serverEnvir(+0x198)+0x1e0 → Server_Envir::m_serverGroup（int，
    // CEnvironment.h 权威布局）。原 movzbl 单字节读为宽度异形，修正为
    // int 读后截断，与 ORIG 指令序列一致。
    unsigned char channelType =
        static_cast<unsigned char>(G_CEnvironment()->m_serverEnvir.m_serverGroup);
    param.m_serverChannelRate =
        chScript->getBonusRate(channelType, channelNo, dungIdx);

    if (user->IsHavePremiumAdvantage())
        param.m_premiumAdvantageRate = (float)(
            user->GetPremiumInfo()->GetAdvantageExpRate() / 100);

    param.m_serverEventExpRate = (float)(CServerEvent::GetExpRate() / 100);
    param.m_manageBonus = CalcurateManageBonus(
        ManageBonusType::T_0, user, (unsigned int)exp);

    unsigned int bonusExp = getClearRewardBonusExp(user, exp, param, bonus);
    if (1000000 < bonusExp)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x3d12, "BLOOD_LOG : EROOR BLOOD CLEAR REWARD EXP!! (%u)\n",
            bonusExp);
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x3d1c,
            "BLOOD_LOG : EXP PARAMETER : %f, %f, %f, %f, %f, %f, %f, %d\n",
            param.m_avatarBonus, param.m_creatureBonus,
            param.m_pcRoomBurningRate, param.m_expDoubleRate,
            param.m_serverChannelRate, param.m_premiumAdvantageRate,
            param.m_serverEventExpRate, param.m_burningFatigueBonusExp);
    }

    // P183：ORIG 该点直接 `add $0xb24` 取址（无引用缓存槽）。
    reinterpret_cast<CBattle_Field*>(&m_battleField)
        ->m_bloodReward.setRewardExp(slot, bonusExp);
