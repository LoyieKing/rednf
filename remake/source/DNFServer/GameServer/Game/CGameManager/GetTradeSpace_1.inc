CTradeSpace* CGameManager::GetTradeSpace(int index)
{
    CTradeSpace* tradeSpace = m_tradeSpacePool.Get(index);
    if (tradeSpace == 0)
        return 0;
    return tradeSpace;
