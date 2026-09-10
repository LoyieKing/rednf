int CMap::insert_monster(STAssignMonster monster, int area)
{
    // ORIG 0x0834e9d4：逐字段拷贝（顺序：mobId/flag04/field08/spawnType(m_20)/
    // levelAdd(m_1c)/field14(m_18)/roleType(m_24)/level(m_14/10)/orderIdx(m_50)/
    // hpPct(m_4c)/flag30(m_54)/mobList(m_vec28)）；area!=-1 时 m_champion=1,
    // m_posX=area（0834ea80/0834ea84）。
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
    item.m_hpPct = monster.m_4c;
    item.m_flag30 = monster.m_54;
    item.m_mobList = monster.m_vec28;
    if (area != -1)
    {
        item.m_champion = 1;
        item.m_posX = area;
    }
    m_monsterList.push_back(item);
    return 1;
