int stQuestShopStatus::getNextLevel(int level)
{
    int count = 0;                                       // -0x0c

    std::vector<stStatusLevelInfo>::iterator it = m_levelInfos.begin(); // -0x18
    while (it != m_levelInfos.end())
    {
        count += static_cast<int>(it->m_second);
        if (count > level)                               // +0x3f cmp/jle 反向
            return static_cast<int>(it->m_first);
        it++;
    }
    return -1;
