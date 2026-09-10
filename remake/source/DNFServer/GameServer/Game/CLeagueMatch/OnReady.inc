bool CLeagueMatch::OnReady(CUser* /*user*/, bool ready)
{
    if (!ready && m_state == 4)
        return false;
    return true;
