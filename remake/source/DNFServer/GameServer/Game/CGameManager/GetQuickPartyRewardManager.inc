QuickParty::CQuickPartyRewardManager* CGameManager::GetQuickPartyRewardManager()
{
    if (m_pQuickPartyRewardMgr == 0)
    {
        m_pQuickPartyRewardMgr = new QuickParty::CQuickPartyRewardManager;
    }
    return m_pQuickPartyRewardMgr;
}