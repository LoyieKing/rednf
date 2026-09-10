void CVillageMonsterMgr::OnRegenVillageMonster(int a, int b)
{
    if (m_field04 == 0)
        return;
    Zone zone((unsigned char)a, (unsigned char)b);
    std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.find(zone);
    // P133：ORIG 0x86b4338 用 operator!=（je 出口正形）非早退 eq。
    if (it != m_areas.end())
    {
        it->second.OnCreateVillageMonster();
    }
