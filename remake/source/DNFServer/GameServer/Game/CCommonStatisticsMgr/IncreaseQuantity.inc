void CStatisticContainer::IncreaseQuantity(int quantity)
{
    int value = m_quantityMap[quantity];
    m_quantityMap[quantity] = ++value;
