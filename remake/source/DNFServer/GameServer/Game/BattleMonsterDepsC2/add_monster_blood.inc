bool CBattle_Field::add_monster_blood(map_monster& mob, int& uid, int& seq)
{
    if (m_pMap == NULL)
        return false;
    if (m_pMap->m_bloodPhaseTime.size() != 0)                             // CMap+0xc4
    {
        m_bloodSystems = m_pMap->m_bloodSystem;                           // +0x240 ← +0xb8
        m_bloodPhaseTimes = m_pMap->m_bloodPhaseTime;                     // +0x24c ← +0xc4
        if (!add_blood_monster(mob, uid, seq, m_pMap->m_bloodSystem,
                               m_pMap->m_bloodPhaseTime))
        {
            LogManager::logFormat(
                1, "battle_field.cpp",
                __PRETTY_FUNCTION__,
                0x62b, "BLOOD_LOG : MONSTERS SETTING ERROR!!\n");
            return false;
        }
    }
    return true;
