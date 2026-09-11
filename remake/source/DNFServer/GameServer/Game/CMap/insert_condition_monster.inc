void CMap::insert_condition_monster(STAssignMonster monster)
{
    // ORIG 0834ead4：仅拷贝 mobId/flag04/field08/spawnType(m_20)/levelAdd(m_1c)/
    // field14(m_18)/roleType(m_24)/level(m_14/10)/orderIdx(m_50)。
    // 不拷贝 m_hpPct/m_flag30/m_mobList/m_champion/m_posX（保持 ctor 默认）。
    _mapMonster item;
    item.m_mobId = monster.m_0;
    item.m_flag04 = monster.m_4;
    item.m_field08 = monster.m_8;
    item.m_spawnType = monster.m_20;
    item.m_levelAdd = monster.m_pair18.m_pair.m_hi;   // +0x10 ← +0x1c
    item.m_field14 = monster.m_pair18.m_pair.m_lo;    // +0x14 ← +0x18
    item.m_roleType = monster.m_24;
    item.m_level = monster.m_14 / 10;
    item.m_orderIdx = monster.m_50;
    m_conditionMonsterList.push_back(item);
}