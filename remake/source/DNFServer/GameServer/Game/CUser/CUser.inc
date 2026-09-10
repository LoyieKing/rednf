CUser::CUser()
    : m_field704a8(0), m_field79670(0), m_field796d8(0), m_chState((ch_state)0)
{
    m_field8ec24 = 0;
    m_field8ec2c = 0;
    m_premium.m_mailBox = 0;
    m_field796d9 = 0;
    m_field8cfd0 = 300;
    m_field8cfd4 = 0;
    m_field8cfd8 = 0;
    setAntibotKey(-1);
    setClientVersion(0);
    m_mcap.m_serialHitCount = 0;
    m_mcap.m_monsterDieHackCnt = 0;
    m_postChecksumData.m_field54 = 0;
    m_field796f8 = 0;
    reset();
    m_historyLog.SetUser(this);
    m_field705d8 = 0;
    m_field8e094 = 0;
    ResetUsedCoinInDungeon();
    m_piSender.m_field10 = _NS_PI_2ND_GetDefaultRandomHashKey();
    m_field8eb18 = _NS_PI_2ND_GetDefaultRandomHashKey();
    m_accountCargo.Clear();
    m_field8eb1c = 0;
    ClearItemSellCount();
    m_field8eb44 = 0;
    m_field8eb48 = 0;
    m_field8eb99 = 0;
    resetInformNoticeFlag();
    disableSaveInformNoticeFlag();
    m_rentalFlag1 = 1;
    m_rentalFlag2 = 0;
    m_rentalInfo.clear();
    m_field8cfcc = (UserSpace::T)0;
    // 2026-09-10（FL2CUser 接收者/字段轴，ORIG 直证）：ORIG ctor 第 390 条为
    //   `movb $0x0,0x7962c(%eax)`（块内 **+0x08** = m_tradePunishOverThirtyDays），
    //   全函数**从不**写 0x79654（块内 +0x30 = m_firstLogin）⇒ 旧形「firstLogin=0」
    //   是发明初始化（多写一条），且真正被 ORIG 清的是 tradePunishOverThirtyDays。
    m_postChecksumData.m_tradePunishOverThirtyDays = 0;   // 块内 +0x08（0x7962c）
    m_advanceAltarIdx = 0xffff;
    m_field8eba8 = 0;
    m_field8ebac = 0;
    m_field8ebb0 = 0;
    m_field8ec29 = 0;
    m_rentalFlag3 = 0;
