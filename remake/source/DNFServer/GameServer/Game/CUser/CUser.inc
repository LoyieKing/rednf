// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864e410 | _ZN5CUserC1Ev
CUser::CUser()
    : m_uniqueId(0), m_beforeAreaTime(0), m_puUser(0), m_chState((ch_state)0)
{
    m_antibotSerialNum = 0;
    m_recipeProbability = 0;
    m_premium.m_mailBox = 0;
    m_guildExpReportLoad = 0;
    m_checkConnInterval = 300;
    m_resumeChecksumTime = 0;
    m_checksumAnswerCount = 0;
    setAntibotKey(-1);
    setClientVersion(0);
    m_mcap.m_serialHitCount = 0;
    m_mcap.m_monsterDieHackCnt = 0;
    m_postChecksumData.m_field54 = 0;
    m_historyLogFp = 0;
    reset();
    m_historyLog.SetUser(this);
    m_field705d8 = 0;
    m_guildCargoCapacity = 0;
    ResetUsedCoinInDungeon();
    m_piSender.m_field10 = _NS_PI_2ND_GetDefaultRandomHashKey();
    m_randomHashKey = _NS_PI_2ND_GetDefaultRandomHashKey();
    m_accountCargo.Clear();
    m_usedCoinCount = 0;
    ClearItemSellCount();
    m_checkPickUpRandomOptionItem = 0;
    m_moveMapHackCnt = 0;
    m_properLevelDungeonUser = 0;
    resetInformNoticeFlag();
    disableSaveInformNoticeFlag();
    m_rentalFlag1 = 1;
    m_rentalFlag2 = 0;
    m_rentalInfo.clear();
    m_moveSpace = (UserSpace::T)0;
    // 2026-09-10（FL2CUser 接收者/字段轴，ORIG 直证）：ORIG ctor 第 390 条为
    //   `movb $0x0,0x7962c(%eax)`（块内 **+0x08** = m_tradePunishOverThirtyDays），
    //   全函数**从不**写 0x79654（块内 +0x30 = m_firstLogin）⇒ 旧形「firstLogin=0」
    //   是发明初始化（多写一条），且真正被 ORIG 清的是 tradePunishOverThirtyDays。
    m_postChecksumData.m_tradePunishOverThirtyDays = 0;   // 块内 +0x08（0x7962c）
    m_advanceAltarIdx = 0xffff;
    m_usedFatigueQuantity = 0;
    m_usedGiftFatigueQuantity = 0;
    m_changedGiftFatigue = 0;
    m_comboSkillSet = 0;
    m_rentalFlag3 = 0;
}