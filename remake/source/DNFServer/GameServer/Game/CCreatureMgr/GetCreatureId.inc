// ORIG _ZN13user_creature9CCreature13GetCreatureIdEi T 0x833769c：
// find_item(itemIdx) 空/堆叠 → -1；否则 m_creatureId = CEquipItem::get_creature_species()
// 并返回它（GetCreatureId.md）。
int CCreature::GetCreatureId(int itemId)
{
    CItem* found = G_CDataManager()->find_item(itemId);
    if (found == 0)
        return -1;
    if (found->is_stackable())
        return -1;
    m_creatureId = static_cast<CEquipItem*>(found)->get_creature_species();
    return m_creatureId;
}
