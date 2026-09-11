CPremiumLetheManager* CGameManager::GetPremiumLetheManager()
{
    if (m_pPremiumLetheMgr == 0)
    {
        m_pPremiumLetheMgr = new CPremiumLetheManager;
    }
    return m_pPremiumLetheMgr;
}