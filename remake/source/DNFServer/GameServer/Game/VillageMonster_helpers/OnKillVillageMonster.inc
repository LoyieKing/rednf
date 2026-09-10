int CVillageMonsterMgr::OnKillVillageMonster(int a, int b, unsigned short c, bool d)
{
    Zone zone((unsigned char)a, (unsigned char)b);
    std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.find(zone);
    if (it == m_areas.end())
        return 0x15;
    return it->second.OnKillVillageMonster(0, c, d);
