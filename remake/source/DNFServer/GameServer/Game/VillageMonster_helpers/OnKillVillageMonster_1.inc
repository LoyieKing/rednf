int CVillageMonsterArea::OnKillVillageMonster(CUser* user, unsigned short zone, bool flag)
{
    std::map<unsigned short, CVillageMonster>::iterator it = m_monsters.find(zone);
    if (it != m_monsters.end())
    {
        CVillageMonster* monster = &it->second;
        int ret = monster->OnKillVillageMonster(user, m_field28, m_field2c, m_field38, flag);
        if (flag && ret == 0)
            m_monsters.erase(it);
        return ret;
    }
    return 0;
