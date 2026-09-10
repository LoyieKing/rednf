bool BindTable_Predicate(                                           // ORIG T 0x832c3cc
    const OUTPUT_ITEM_STR& a, const OUTPUT_ITEM_STR& b)
{
    if (a.m_usableLevel == b.m_usableLevel)
    {
        return a.m_index < b.m_index;
    }
    return a.m_usableLevel < b.m_usableLevel;
