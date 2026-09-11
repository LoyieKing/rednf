// ORIG 0x08301bf6：血战系统/阶段时间自 CMap 复制到本对象后转调 add_blood_monster。
// [b28d-solo-BattleMonsterDepsC2] P197④/P225/P0：ORIG `call size; test %eax; setne %al;
// test %al; je` ⇒ 源写 `size() != 0`（empty() 生 test %al 直跳）；且 ORIG 无 ok 具名局部
// （`mov %eax,%ebx; xor $1,%ebx` 保活跨析构）⇒ 条件内联表达式，去掉 `const bool ok` 槽。
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
}