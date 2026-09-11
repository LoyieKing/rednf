int CTraceMobDieHack::addMonsterDamage(int nMonsterIndex, unsigned short monsterId, int damage)
{
    stMonsterDamageLog_t log;
    log.m_monsterId = monsterId;
    log.m_damage = (unsigned short)damage;
    log.m_count = 1;
    log.m_monsterIndex = nMonsterIndex;

    // map 的 value_type：pair<const unsigned short, stMonsterDamageLog_t>
    std::pair<std::map<unsigned short, stMonsterDamageLog_t>::iterator, bool>
        result = m_map1.insert(std::make_pair(monsterId, log));
    if (!result.second)
    {
        if (m_map1[monsterId].m_damage == damage)
        {
            m_map1[monsterId].m_count = m_map1[monsterId].m_count + 1;
        }
        else
        {
            m_map1[monsterId] = log;
        }
    }
    return m_map1[monsterId].m_count;
}