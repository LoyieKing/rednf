void CUser::CalcurateUserMaxLevel()
{
    int maxLevel = 0;
    // ORIG 0x867f??：`cmp -0xc(%ebp),%eax; setg`（signed 比较，i 为 int）+
    // operator[] 实参 idx 先载 %eax、this 走 %edx mov+add（技巧 15 失配序）。
    for (int i = 0; i < (int)m_characList.size(); ++i)
    {
        if ((int)m_characList[i].m_level > maxLevel)
            maxLevel = m_characList[i].m_level;
    }
    if (maxLevel > 70)
        maxLevel = 70;
    SetUserMaxLevel(maxLevel);
