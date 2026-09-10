Secu_DungeonAverageCheck::findKillMonsterInfo(unsigned int monsterId)
{
    // ORIG 0x827993c：线性查找 m_monsterId 匹配项，未命中返回 end()。
    for (std::vector<SecKillMonsterInfo>::iterator it = m_killInfos.begin();
         it != m_killInfos.end(); ++it)
    {
        if ((unsigned int)it->m_monsterId == monsterId)
            return it;
    }
    return m_killInfos.end();
