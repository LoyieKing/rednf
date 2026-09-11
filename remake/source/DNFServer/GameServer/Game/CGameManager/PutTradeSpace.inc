void CGameManager::PutTradeSpace(CTradeSpace* tradeSpace)
{
    if (tradeSpace == 0)
        return;
    tradeSpace->reset();
    m_tradeSpacePool.Free(tradeSpace);
}