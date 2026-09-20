void CUserCharacInfo::setCurCharChangingGrowthType(char first, char second)
{
    if (first > 6)
        return;
    if (second > 2)
        return;
    m_selected->m_pad2a.m_sub = first;
    m_selected->m_pad2a.m_grow = second;
}
