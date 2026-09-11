// ============================================================================
// CVillageMonsterMgr::OnRegenVillageMonster —— ORIG 0x86b4338
//   未开战返回；Zone(a,b) 命中区域则补生成一只。
// ============================================================================
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
}