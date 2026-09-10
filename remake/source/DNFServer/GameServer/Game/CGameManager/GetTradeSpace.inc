CTradeSpace* CGameManager::GetTradeSpace()
{
    CTradeSpace* tradeSpace = m_tradeSpacePool.Acquire();
    if (tradeSpace == 0)
        return 0;
    tradeSpace->SetIDX(m_tradeSpacePool.GetIndex(tradeSpace));
    return tradeSpace;
