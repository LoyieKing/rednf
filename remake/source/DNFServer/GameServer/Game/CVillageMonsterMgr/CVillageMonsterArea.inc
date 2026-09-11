// ============================================================================
// CVillageMonster area 构造 —— ORIG 0x86b3752
// ============================================================================
CVillageMonsterArea::CVillageMonsterArea(int a, int b, int c, int d,
                                         std::vector<STAttackedMonster>* monsters,
                                         std::vector<MapArea>* areas)
{
    // ORIG 0x86b3752：map/vector 默认构造后按偏移赋值。
    m_field28 = a;                       // +0x28
    m_field2c = b;                       // +0x2c
    m_monsterList = *monsters;           // +0x1c vector 拷贝
    m_areaList = areas;                  // +0x18
    // N76 封顶（ndiff1）：ORIG 0x86b3752 此句先备值(c→edx)后载 this；4.4 对 int 参数
    // mem→mem store 恒 LHS-first，探针直成员/视图/链式/中间量全不可翻（enum 参数才值先）。
    m_field34 = c;                       // +0x34
    m_field38 = d;                       // +0x38
    m_field30 = ENUM_VILLAGE_MONSTER_ROTATE_0;   // +0x30
}