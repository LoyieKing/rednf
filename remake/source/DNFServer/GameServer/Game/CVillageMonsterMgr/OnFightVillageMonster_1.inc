// ============================================================================
// CVillageMonsterArea::OnFightVillageMonster —— ORIG T 0x86b3fe2
//   zone 未命中 → 0x15；否则转交怪物（out1/out2 引用透传）。
// ============================================================================
int CVillageMonsterArea::OnFightVillageMonster(CUser* user, unsigned short zone,
                                               int& out1, int& out2)
{
    std::map<unsigned short, CVillageMonster>::iterator it =
        m_monsters.find(zone);
    // P0：ORIG 0x86b3fe2 有 -0xc 槽 ⇒ 原码有具名 monster 指针局部；
    // ne 正形 + 尾部 0x15 块（je +0x87）。
    if (it != m_monsters.end())
    {
        CVillageMonster* monster = &it->second;
        return monster->OnFightVillageMonster(user, out1, out2);
    }
    return 0x15;
}