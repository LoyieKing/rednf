void CParty::set_clear_reward_exp_parameter(CUser* user, unsigned int exp,
                                            stClearRewardExpParameter_t& param)
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS G_CDataManager（1 次，局部 dm 已存在但少一次重调）。@0x85adb00
    CDataManager* dm = G_CDataManager();                    // -0x38 local_3c
    int dungeonIndex =
        reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_index();
    CDungeon* dungeon = dm->find_dungeon(dungeonIndex);     // -0x34 local_38
    ServerParameterScript* spScript = &dm->m_serverParameter;  // -0x30 local_34（+0x68）

    if (dungeon == 0)
    {
        LogManager::logFormat(
            1, "party.cpp",
            __PRETTY_FUNCTION__,
            0x294a, "Unknown Dungeon %d",
            reinterpret_cast<CBattle_Field*>(&m_battleField)->get_dungeon_index());
        return;
    }
    if (user == 0)
    {
        return;
    }

    // +0x096..+0x119：四项基础加成
    float avatarBonus = CalcAvatarBonus();                  // -0x2c local_30

    float pcRoomBurningRate = 0.0f;                         // -0x28 local_2c
    {
        CEventBase* ev8 = GlobalData::s_event_manager->GetRepeatEvent(8);
        if (ev8->IsEventing(user))
        {
            CPCRoomBurningEvent* burning = static_cast<CPCRoomBurningEvent*>(
                GlobalData::s_event_manager->GetRepeatEvent(8));
            pcRoomBurningRate = static_cast<float>(burning->GetExpFactor() / 100);
        }
    }

    float expDoubleRate = 0.0f;                             // -0x24 local_28
    {
        CEventBase* ev3 = GlobalData::s_event_manager->GetRepeatEvent(3);
        if (ev3->IsEventing(static_cast<CUser*>(0)))
        {
            CExpDoubleEvent* doubleEv = static_cast<CExpDoubleEvent*>(
                GlobalData::s_event_manager->GetRepeatEvent(3));
            expDoubleRate = static_cast<float>(doubleEv->GetExpFactor()) / 100.0f;
        }
    }

    float periodItemRate = 0.0f;                            // -0x20 local_24
    if (user->IsExpItemAffected() && user->EnablePeriodExpItem())
    {
        periodItemRate = user->GetExpAffectRate();
    }

    int pcRoomSameUserBonus = 0;                            // -0x1c local_20
    // ORIG 0x085adc8a/0x085adc9f：GetPCRoomNo 两次独立调用（P48），G_CDataManager 二次直呼
    if (static_cast<unsigned int>(user->GetPCRoomNo()) != 0 &&
        existSamePCRoomUser(static_cast<unsigned int>(user->GetPCRoomNo())))
    {
        pcRoomSameUserBonus = G_CDataManager()->m_serverParameter.m_76c;  // +0x7d4
    }

    float returnUserBonus = 0.0f;                           // -0x18 local_1c
    if (IsReturnUserParty())
    {
        returnUserBonus = 0.2f;                             // ORIG 0x3e4ccccd
    }

    // +0x21a..+0x27c：param 基础字段回填
    param.m_deadPenaltyRate = calDeadUserDungeonPenalty(param.m_deadMemberCount);
    param.m_rankBonusWeight = spScript->GetExpBonusWeight(
        static_cast<eRankBonusIndex>(param.m_rankBonusIdx));
    param.m_avatarBonus = avatarBonus;
    param.m_creatureBonus = CalcCreatureBonus(user);
    param.m_pcRoomBurningRate = pcRoomBurningRate;
    param.m_expDoubleRate = expDoubleRate;

    // +0x27f..+0x2d5：燃烧疲劳奖励
    CBurningFatigueEvent* fatigueEvent = static_cast<CBurningFatigueEvent*>(
        GlobalData::s_event_manager->GetRepeatEvent(0xf));
    if (fatigueEvent->IsEventing(static_cast<CUser*>(0)))
    {
        param.m_burningFatigueBonusExp =
            fatigueEvent->getBonusExpRate(user->getCurCharacUsedFatigue());
    }

    // +0x2d8..+0x394：频道加成（服务器类型 1/2 走 getBonusRateEventServer）
    // ORIG 0x085addd3 第三次独立 call G_CDataManager（旧码复用 dm ⇒ p167 MISS 1）。
    channel_script_t* channelScript =
        static_cast<channel_script_t*>(G_CDataManager()->GetChannelScript());
    // ORIG 0x085addf3..0x085ae094：G_CEnvironment 各判定点独立重读（P48），bool 物化
    bool eventServer =
        (G_CEnvironment()->m_serverEnvir.m_serverType == 1) ||
        (G_CEnvironment()->m_serverEnvir.m_serverType == 2);
    if (eventServer)
    {
        param.m_serverChannelRate = channelScript->getBonusRateEventServer(
            (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
            static_cast<unsigned int>(G_CEnvironment()->get_channel_no()));
    }
    else
    {
        param.m_serverChannelRate = channelScript->getBonusRate(
            (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
            static_cast<unsigned int>(G_CEnvironment()->get_channel_no()),
            static_cast<unsigned int>(dungeon->get_index()));
    }

    // +0x397..+0x3cd：会员/服务器/全局效果加成
    if (user->IsHavePremiumAdvantage())
    {
        param.m_premiumAdvantageRate =
            static_cast<float>(user->GetPremiumInfo()->GetAdvantageExpRate()) /
            100.0f;
    }
    param.m_serverEventExpRate =
        static_cast<float>(CServerEvent::GetExpRate()) / 100.0f;
    if (GlobalData::s_globalEffectManager_->isAffectedEffect(2))
    {
        param.m_globalEffectRate =
            GlobalData::s_globalEffectManager_->getEffectValue(2);
    }

    // +0x41a..+0x429：网吧同号/回归用户奖励（dword 位型写入）
    *reinterpret_cast<int*>(&param.m_pcRoomSameUserBonus) = pcRoomSameUserBonus;
    param.m_returnUserPartyBonus = returnUserBonus;

    // +0x42c..+0x474：事件角色队伍经验比例
    if (!IsEventCharacParty())
    {
        param.m_eventCharacExpRatio = 0.0f;
    }
    else
    {
        EventNewCharacterReward* newCharEv = reinterpret_cast<EventNewCharacterReward*>(
            GlobalData::s_event_manager->GetRepeatEvent(0xa2));
        if (newCharEv != 0)
        {
            param.m_eventCharacExpRatio = newCharEv->eventExpRatio();
        }
    }

    // +0x477..+0x498：管理加成
    param.m_manageBonus =
        CalcurateManageBonus(static_cast<ManageBonusType::T>(0), user, exp);

    // +0x49e..+0x4dd：成长之力（STExpReward 局部回填 dword0 后取字节 2）
    STExpReward growthTmp;                                     // -0x3c
    growthTmp.m_reward =
        user->get_growth_power_exp_reward().m_reward;          // sret dword → +0x00
    param.m_growthPowerExpRate =
        static_cast<float>((growthTmp.m_reward >> 16) & 0xff) / 100.0f;

    // +0x4e0..+0x530：会员额外经验（premium type 0x4f）
    if (!user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0x4f)))
    {
        param.m_premiumAddExpRate = 0.0f;
    }
    else
    {
        // ORIG 0x085ae005：直呼 CUser::getAddExpRateFromPremium(ENUM_PREMIUM_TYPE)（W 0x085bfd76）
        param.m_premiumAddExpRate =
            static_cast<float>(
                user->getAddExpRateFromPremium(static_cast<ENUM_PREMIUM_TYPE>(0x4f))) /
            100.0f;
    }

    // +0x533..+0x57b：周期经验道具（与重叠加成事件互斥清零）
    param.m_periodExpItemRate = periodItemRate;
    if (param.m_periodExpItemRate != 0.0f &&
        GlobalData::s_event_manager->IsExistOverlappedExpBonusEvent())
    {
        param.m_periodExpItemRate = 0.0f;
    }

    // +0x57b..+0x5c7：清零四项角色 bonus exp
    user->set_charac_party_bonus_exp(0);
    user->set_charac_member_bonus_exp(0);
    user->set_charac_fatigue_buf_bonus_exp(0);
    user->set_charac_seria_buf_bonus_exp(0);
