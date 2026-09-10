void CRelayBattleMgr::OnReady(int seat, bool ready)
{
    if (seat < 0)
        return;
    if (seat > 7)
        return;
    m_battles[seat].m_state = ready ? 1 : 0;
