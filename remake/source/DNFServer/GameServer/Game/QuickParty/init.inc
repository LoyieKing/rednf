void CQuickParty::init()
{
    memset(m_partyIndex, -1, sizeof(m_partyIndex));   // ORIG memset(this,-1,0x10)
    m_quickPartyIndex = -1;
    m_firstMatchingCnt = 0;
    m_dungeonIndex = 0;
    m_dungeonDiff = (char)0xff;
    m_poolIndex = (char)0xff;
    for (int msg = 0x9d; msg <= 0x9e; ++msg)
        m_timerKeyCnt[msg - 0x9d] = 0;
    m_isChangeQuickPartyCount = 0;
