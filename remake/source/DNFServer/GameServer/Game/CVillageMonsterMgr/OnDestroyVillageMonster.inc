// ============================================================================
// CVillageMonsterArea::OnDestroyVillageMonster —— ORIG T 0x86b3ee0
//   清空怪物表并广播空列表包。
// ============================================================================
void CVillageMonsterArea::OnDestroyVillageMonster()
{
    m_monsters.clear();
    SendVillageMonsterList(0);
}