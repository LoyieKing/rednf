// ============================================================================
// CVillageMonsterArea::OnRunVillageMonster —— ORIG T 0x86b3f06
//   区域无人 → 直接返回；否则逐怪物转 OnRunVillageMonster(m_areaList, m_field30)，
//   然后 SendVillageMonsterPosition 并 ++m_field30（轮转槽）。
// ============================================================================
// [b28d-solo-CVillageMonsterMgr] AE（ORIG 0x086b3f06）：加具名 monster 指针
// 局部（复现 ORIG -0xc 槽 + ptE 先于实参求值）；m_field30 经
// `(ENUM_VILLAGE_MONSTER_ROTATE)m_field30` 实参 cast 复现两段装载（P265），
// 字段保持 int 以保 `++` 的 lea 形。p167 CLEAR。
void CVillageMonsterArea::OnRunVillageMonster()
{
    if (G_GameWorld()->GetUserCount(m_field28, m_field2c) == 0)
        return;
    for (std::map<unsigned short, CVillageMonster>::iterator it =
             m_monsters.begin();
         it != m_monsters.end(); ++it)
    {
        CVillageMonster* monster = &it->second;
        monster->OnRunVillageMonster((std::vector<MapArea>*)m_areaList,
                                     m_field30);
    }
    SendVillageMonsterPosition();
    m_field30 = (ENUM_VILLAGE_MONSTER_ROTATE)(m_field30 + 1u);
}