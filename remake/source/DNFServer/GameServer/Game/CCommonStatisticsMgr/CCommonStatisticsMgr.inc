CCommonStatisticsMgr::CCommonStatisticsMgr()
{
    m_container[0] = new CHelpedPartyCount;
    m_container[1] = new CPVPPlayerRate;
    m_container[2] = new CPVPMapCount;
