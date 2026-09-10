int stQuestShopStatus::getMaxCount(int level)
{
    int count = 0;                                       // -0x0c

    std::vector<stStatusLevelInfo>::iterator it = m_levelInfos.begin(); // -0x18（ORIG 循环外 begin）
    while (it != m_levelInfos.end())                     // 循环头 end() + operator!=
    {
        if (static_cast<int>(it->m_first) <= level)      // cmp 0xc(%ebp),%eax
            count += static_cast<int>(it->m_second);     // add m_max
        it++;                                            // ORIG 后置 operator++(int)
    }
    return count;
