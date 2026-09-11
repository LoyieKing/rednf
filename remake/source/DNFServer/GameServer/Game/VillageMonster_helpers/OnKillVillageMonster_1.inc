// ============================================================================
// CVillageMonsterArea::OnKillVillageMonster(CUser*,unsigned short,bool)
//   —— ORIG 0x86b4070
//   objdump: m_monsters.find(zone); it != end 时:
//            ret = it->second.OnKillVillageMonster(user, +0x28, +0x2c, +0x38, flag);
//            if (flag && ret == 0) m_monsters.erase(it);
//            return ret;  否则 return 0
// ============================================================================
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF → n1_v 新口径 AE（gap=+0，blocks=0）⇒ 旧判词作废。
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
}