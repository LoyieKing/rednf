void CUserCharacInfo::setCurCharGrowthType(char first, char second)
{
    if (first > 6)
        return;
    if (second > 2)
        return;
    enableSaveCharacInfo();
    m_selected->m_field29.m_sub = first;
    m_selected->m_field29.m_grow = second;
}
