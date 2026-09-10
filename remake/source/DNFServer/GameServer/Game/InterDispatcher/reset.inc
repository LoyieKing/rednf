void Inter_NpcLimitBuyItemInfo::reset() throw()
{
    m_error = 0;
    m_needItem.first = 0;
    m_needItem.second = 0;
    m_param1 = -1;
    m_param2 = -1;
    m_itemRef = 0;
    m_data1c = 0;
    m_invenItem1.reset();
    m_invenItem2.reset();
