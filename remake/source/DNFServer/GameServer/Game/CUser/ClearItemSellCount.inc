void CUser::ClearItemSellCount()
{
    m_itemSellCount = 0;
    memset(m_sellCountData, 0, sizeof(m_sellCountData));
}