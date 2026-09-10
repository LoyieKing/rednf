int CCreature::GetCreatureId(int itemId)
{
    CItem* found = G_CDataManager()->find_item(itemId);
    if (found == 0)
        return -1;
    if (found->is_stackable())
        return -1;
    m_creatureId = static_cast<CEquipItem*>(found)->get_creature_species();
    return m_creatureId;
