// ============================================================================
// 生命周期 / 房间钩子
// ============================================================================
void CRelayBattleMgr::Reset()
{
    for (int i = 0; i < 8; ++i)
    {
        m_battles[i].Reset();
    }
    ResetChangeSequenceCount();
    m_lastFightStartTime = 0;
    m_state = 0;
}