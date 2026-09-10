void CParty::SetPartyInfoUI(MSG_PARTY_SET_INFO const& info)
{
    setTitle(info.m_title);
    setTitleIndex(info.m_titleIndex);
    setUserMax(info.m_userMax);
    setDungIndex(info.m_dungIndex);
    setDungDiffi(info.m_dungDiffi);
