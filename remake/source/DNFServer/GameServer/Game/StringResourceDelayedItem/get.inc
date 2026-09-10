bool StringResourceDelayedItem::get(CharString const** out) const
{
    return m_pRes->get(m_id, out);
