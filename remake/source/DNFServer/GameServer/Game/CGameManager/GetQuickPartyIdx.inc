int CGameManager::GetQuickPartyIdx(QuickParty::CQuickParty* qp)
{
    int idx = m_quickPartyPool.GetIndex(qp);
    return idx;
