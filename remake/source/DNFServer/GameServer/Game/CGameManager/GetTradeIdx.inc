int CGameManager::GetTradeIdx(CTradeSpace* tradeSpace)
{
    int idx = m_tradeSpacePool.GetIndex(tradeSpace);
    return idx;
}