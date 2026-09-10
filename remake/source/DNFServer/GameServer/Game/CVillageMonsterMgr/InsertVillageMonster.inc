void CVillageMonsterMgr::InsertVillageMonster(int a, int b, int c, int d,
                                              std::vector<STAttackedMonster>* monsters,
                                              std::vector<MapArea>* areas)
{
    // ORIG 0x86b4144：构造区域后以 Zone(a,b) 为键插入 m_areas。
    // P210（本件实证）：全临时内联形使 4.4 栈槽排布复现 ORIG（area -0x48 / zone
    // -0xa / mkpair -0x88 / pair_cc -0xc8 / insert 返回 -0xd0）；具名局部形 4.4 将
    // 具名对象压帧底（-0xd0/-0x92），逐条错位。
    m_areas.insert(std::make_pair(Zone((unsigned char)a, (unsigned char)b),
                                  CVillageMonsterArea(a, b, c, d, monsters, areas)));
