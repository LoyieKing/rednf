bool CItem::IsPriorityJob(int job, int growType)
{
    if (m_vecC0.empty())
        return false;

    int normalizedGrowType = 0;
    if (growType == 1 || growType == 0x11)       // ORIG cmp/je+cmp/jne 链
        normalizedGrowType = 1;
    else if (growType == 2 || growType == 0x12)
        normalizedGrowType = 2;
    else if (growType == 3 || growType == 0x13)
        normalizedGrowType = 3;
    else if (growType == 4 || growType == 0x14)
        normalizedGrowType = 4;

    for (std::vector<std::pair<int, int> >::const_iterator it = m_vecC0.begin();
         it != m_vecC0.end(); it++)
    {
        const std::pair<int, int>& info = *it;
        if (info.first == job && info.second == normalizedGrowType)
            return true;
    }
    return false;
