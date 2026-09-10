void CCommonStatisticsMgr::IncreaseQuantity(ENUM_STATISTIC_CONTAINER container,
                                            int quantity)
{
    if (container < 0 || container > 2)
        return;
    m_container[container]->IncreaseQuantity(quantity);
