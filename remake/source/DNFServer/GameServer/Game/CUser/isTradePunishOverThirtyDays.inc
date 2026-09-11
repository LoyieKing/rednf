bool CUser::isTradePunishOverThirtyDays()
{
    return m_postChecksumData.m_tradePunishOverThirtyDays;   // +0x7962c（ORIG movzbl 直返；原误读 m_firstLogin）
}