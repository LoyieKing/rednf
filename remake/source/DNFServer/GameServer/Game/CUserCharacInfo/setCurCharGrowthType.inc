void CUserCharacInfo::setCurCharGrowthType(char first, char second)
{
    if (first > 6)
        return;
    if (second > 2)
        return;
    enableSaveCharacInfo();
    CUR->m_growType.first = first;
    CUR->m_growType.second = second;
