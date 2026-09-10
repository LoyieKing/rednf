bool CDungeonNode::UpdateMonster(SIG_DUNGEON_DATA_STATISTIC* stat)
{
    std::map<int, Monster>::iterator it = m_monsterMap.find(stat->m_monsterKey);
    if (it == m_monsterMap.end())
        return false;
    Monster& m = it->second;
    m += *reinterpret_cast<const Monster*>(stat->m_monster);
    return true;
