// ============================================================================
// 管理器
// ============================================================================
QuickParty::CQuickPartySystemManager* CGameManager::GetQuickPartySystemManager()
{
    if (m_pQuickPartySystemMgr == 0)
    {
        m_pQuickPartySystemMgr = new QuickParty::CQuickPartySystemManager;
    }
    return m_pQuickPartySystemMgr;
}