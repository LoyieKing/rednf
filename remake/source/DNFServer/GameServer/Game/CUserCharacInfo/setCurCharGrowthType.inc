void CUserCharacInfo::setCurCharGrowthType(char first, char second)
{
    if (first > 6)
        return;
    if (second > 2)
        return;
    enableSaveCharacInfo();
    m_selected->m_growthType.m_sub = first;
    m_selected->m_growthType.m_grow = second;
}
