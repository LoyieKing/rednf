void CRelayBattleMgr::OnFight()
{
    m_lastFightStartTime = 0;
    ArrangeMatchSequence();
    TurnPlayer();
    m_state = 1;
}