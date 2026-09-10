CConditionEventManager* CGameManager::GetConditionEventManager()
{
    if (m_pConditionEventMgr == 0)
    {
        m_pConditionEventMgr = new CConditionEventManager;
    }
    return m_pConditionEventMgr;
