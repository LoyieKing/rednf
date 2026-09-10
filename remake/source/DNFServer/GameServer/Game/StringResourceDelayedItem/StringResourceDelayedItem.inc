StringResourceDelayedItem::StringResourceDelayedItem(StringResource const& res, int id)
    : m_pCache(0)
{
    m_pRes = &res;
    m_id = id;
