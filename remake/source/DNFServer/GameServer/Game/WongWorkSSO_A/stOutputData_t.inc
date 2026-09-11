// ---- stOutputData_t::stOutputData_t()  ORIG W 0x0818fb28：逐字段置零，
//      m_loginIp(+0x18)/pad 区仅清首字节，与 ORIG 指令序一致。
WongWork::CSimpleSSO::stOutputData_t::stOutputData_t()
{
    m_lastPlayTime = 0;
    m_accountFail = 0;
    m_totalAccountFail = 0;
    m_reportCnt = 0;
    m_reliableFlag = 0;
    m_tradeGoldDaily = 0;
    m_loginIp[0] = 0;
    m_securityFlag = 0;
    m_dungeonGainGold = 0;
    m_schoolId = 0;
    m_rating = 0.0f;
    m_tutorialSkipable = 0;
    m_eventCharacFlag = 0;
    m_loginTime = 0;
    m_expireTime = 0;
}