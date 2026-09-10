void SigLoadRewardEventItem::set(unsigned int v)
{
    m_field0 = v;
    m_count = 0;
    memset(m_characNos, 0, sizeof(m_characNos));
    memset(m_eventCodes, 0, sizeof(m_eventCodes));
