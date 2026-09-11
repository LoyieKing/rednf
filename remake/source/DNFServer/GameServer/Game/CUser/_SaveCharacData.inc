// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08650aa2 | _ZN5CUser15_SaveCharacDataEv
void CUser::_SaveCharacData()
{
    SetAccountLastPlayTime(GlobalData::s_systemTime_.getCurSec());

    if (getCurCharacVill() == 7)   // power war 村
    {
        setCurCharacLastPlayTickPowerWar(GlobalData::s_systemTime_.getCurSec());
        SetCurCharacChaosDieTime(0);
        m_characterView.enableSaveCharacView();
    }
    setCurCharacLastPlayTick(GlobalData::s_systemTime_.getCurSec());

    if (!isSaveCharacInfo() && !IsSaveCharacStat() && !isSaveCharacEtc() &&
        !IsSaveCharacManageInfo())
        return;

    // 本件反例（_SaveCharacData）：ORIG 0x0864a5xx 有 `mov %eax,-0x20(%ebp)` +
    // `mov -0x20(%ebp),%eax`（Acquire 结果**有**具名槽）⇒ 保留 Stream* 局部。
    Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x19c3);
    CStreamGuard guard(s, true);
    // b28d-CUser p167 真缺口：ORIG 显式调 CStreamGuard::operator*（deEv）×2 与
    //   operator->（ptEv）×1 再 GetInBuffer（P132/P103）；直接成员调用少 3 个调用点。
    (*guard)->operator<<((int)0x22);         // SIG_SAVE_CHARAC
    (*guard)->operator<<((int)GetUID());
    SIG_SAVE_CHARAC* sig =
        guard.operator->()->GetInBuffer<SIG_SAVE_CHARAC>();
    memset(sig, 0, sizeof(SIG_SAVE_CHARAC));

    sig->m_dungeonGainGold = getDungeonGainedGold()->getGainedGold();
    sig->m_ssoId = (unsigned int)get_acc_id();
    sig->m_reportCnt = m_field8d110;   /* ORIG this+0x8d110 实证 */
    sig->m_ssoTradeGoldDaily = GetOverTradeGoldCount();
    sig->m_rating = GetRating();              // ORIG fstps 0xf6（4B float 实证）
    sig->m_tutorialSkipable = GetTutorialSkipable();
    sig->m_characNo = getCurCharacNo();

    sig->m_bSaveInfo = isSaveCharacInfo();
    sig->m_bSaveStat = IsSaveCharacStat();
    sig->m_bSaveInfoSub = isSaveCharacEtc();
    sig->m_bSaveManageInfo = IsSaveCharacManageInfo();
    sig->m_bSaveCubePremiumInfo = isSavedPremiumCubeInfo();
    if (sig->m_bSaveCubePremiumInfo)
    {
        // ORIG 0x08650d30/0x08650d39 lea 直传包内字段地址（0x10b/0x10c）。
        getContractOfCubePremiumInfo(sig->m_cubeSelected, sig->m_cubeType);
    }
    if (sig->m_bSaveInfo)   // info 块
    {
        sig->m_lev = get_charac_level();             // ORIG mov %dx（2B）
        sig->m_growType = getCurCharacGrowType();
        // ORIG 0x08650d8c..0x08650da4：0x52 字节 _Additioal_info **结构整体赋值**
        // （对齐 peel + rep movsl 内联，无 memcpy call；源 _Charac_info+0x88）。
        *(_Additioal_info*)&sig->m_maxHP =
            getCurCharacR()->m_additioalInfo;
        sig->m_expertJob = GetCurCharacExpertJobType();
    }
    if (sig->m_bSaveStat)   // stat 块
    {
        sig->m_exp = getCurCharacExp();
        sig->m_village = getCurCharacVill4Save();
        sig->m_fatigue = GetFatigue();        // ORIG mov %ax（2B）
        sig->m_premiumFatigue = getCurCharacPremiumFatigue();
        sig->m_hp = getCurCharacStamina();
        sig->m_usedFatigue = getCurCharacUsedFatigue();
        sig->m_tutorialFlag = getCurCharacTutorialFlag();
        if (getCurCharacR() != 0)
            memcpy(sig->m_escaladeTutorialFlag, getCurCharacEscaladeTutorialFlag(), 0x10);
        sig->m_tradeGoldTotal = getCurCharacTradeGoldTotal();
        sig->m_tradeGoldTotalBillion = getCurCharacTradeGoldTotalBillion();
        sig->m_tradeGoldDaily = getCurCharacTradeGoldDaily();
        sig->m_helpAbuseRatio = getCurCharacDungeonHelpAbuseRatio();
        sig->m_helpAbuseExp = getCurCharacDungeonHelpAbuseExp();
        sig->m_chaosPoint = GetCurCharacChaosPoint();
        sig->m_chaosExp = GetCurCharacChaosExp();
        sig->m_chaosModeCount = GetCurCharacChaosModeCount();
        sig->m_chaosKillCount = GetCurCharacChaosKillCount();
        sig->m_chaosDieCount = GetCurCharacChaosDieCount();
        sig->m_chaosDieTime = GetCurCharacChaosDieTime();
        sig->m_openFlag = GetOpencoin();
        sig->m_assaultCount = GetAssaultCount();
        sig->m_chaosKillTime = GetCurCharacChaosKillTime();
        sig->m_luckPoint = GetCurCharacLuckPoint();
        sig->m_dungeonPlayCount = GetCurCharacDungeonPlayCount();
        sig->m_expertJobExp = GetCurCharacExpertJobExp();
        sig->m_fatigueBatteryCharging = GetFatigueBatteryCharging();
        sig->m_powerWarPoint = GetUserPowerWarPoint();
        sig->m_villagePrev = getCurCharacVillPrev();
        sig->m_powerWarAssaultCount = GetPowerWarAssaultCount();
        sig->m_powerWarAssaultVictoryCount = GetPowerWarAssaultVictoryCount();
        sig->m_fatigueGrownupBuff = getCurCharacFatigueGrownUpBuff();
        sig->m_growthPowerReward = get_growth_power_reward();
        sig->m_memberDungeonFlag = getMemberDungeonFlag();
        sig->m_memberBonusFatigue = getCurCharacMemberBonusFatigue();
        sig->m_lastPlayDungeonIndex = getCurCharacLastPlayDungeonIndex();
        sig->m_chaosResponTime = GetCurCharacChaosResponTime();
        sig->m_addEquipslotFlag = getCurEquipslotFlag();
        sig->m_channelEquipslotSwitch = getCurChannelEquipslotSwitch();
        sig->m_expandEquipslotSwitch = getCurExpandEquipslotSwitch();
        sig->m_lastPlayTimePowerwar = getCurCharacLastPlayTickPowerWar();
        sig->m_emotion = GetCurCharacEmotion();
        sig->m_skillTreeIndex = GetCurCharacSkillTreeIndex();
        sig->m_addSlotFlag = GetCurExEquipSlotStat();
        getCurCharacInvenW()->GetAvatarItemMgrW()->SaveEndurance();
    }
    if (sig->m_bSaveInfoSub)   // etc 块
    {
        sig->m_guildRight = get_guild_create_right();
        sig->m_maxPremiumFatigue = getCurCharacMaxPremiumFatigue();
        sig->m_eventCharacLevel = getCurCharacEventCharacterGrowtype();
    }
    if (sig->m_bSaveManageInfo)   // manage info 块
    {
        sig->m_strikerSkillIndex = getCurCharacStrikerSkillIndex();
        sig->m_tagCharacNo = getCurCharacTagCharacNo();
        sig->m_maxEquipLevel = GetCurCharacMaxEquipLevel();
    }

    sig->m_lastPlayTime = getCurCharacLastPlayTick();
    sig->m_visibleFlags = get_charac_visible_values();
    // ORIG 0x08651298：显式调全局 setBit_Byte(&sig->m_visibleFlags, false, 0)
    // （movb 写 + lea 0x102(%eax) 取址；非 &= ~0x01 直算）。
    setBit_Byte((unsigned char&)sig->m_visibleFlags, false, 0);

    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    disableSaveCharacInfo();
    disableSaveCharacStat();
    disableSaveCharacEtc();
    disableSaveCharacManageInfo();
    disableSavedPremiumCubeInfo();
}