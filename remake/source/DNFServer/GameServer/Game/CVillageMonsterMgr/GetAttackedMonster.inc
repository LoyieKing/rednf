STAttackedMonster* CVillageMonsterArea::GetAttackedMonster(int idx)
{
    if (m_monsterList.empty())
        return 0;
    std::vector<STAttackedMonster>::iterator it = m_monsterList.begin();
    std::vector<STAttackedMonster>::iterator endIt = m_monsterList.end();
    for (; it != endIt; ++it)
    {
        if ((*it).m_attackWeight >= idx)
            return &*it;
    }
    --it;
    return &*it;
