void CRelayBattleMgr::OnLeaveRoom(int seat)
{
    if (seat < 0)
        return;
    if (seat > 7)
        return;
    if (m_battles[seat].m_state == 2)
    {
        OnDiePlayer(seat, -1, true);
    }
    m_battles[seat].Reset();
