void CUser::SetTradeSpace(int idx)
{
    lock();
    m_tradeSpaceIdx = (short)idx;
    unlock();
