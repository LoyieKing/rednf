void CUserCharacInfo::setCurCharChangingGrowthType(char first, char second)
{
    if (first > 6)
        return;
    if (second > 2)
        return;
    CUR->m_growTypeChanging.first = first;
    CUR->m_growTypeChanging.second = second;
