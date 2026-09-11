CSpecialItemRoutingManager* CGameManager::GetSpecialItemRoutingManager()
{
    if (m_pSpecialItemRoutingMgr == 0)
    {
        m_pSpecialItemRoutingMgr = new CSpecialItemRoutingManager;
    }
    return m_pSpecialItemRoutingMgr;
}