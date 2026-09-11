// ORIG 0x833f618：GetCreatureItemType()==1 → 归还 m_creaturePool(+0x5c)；
// ==0 → 归还 m_eggPool(+0x00)；否则返回 0（ORIG 无 null 检查，忠实转写）。
bool user_creature::CCreatureItemFactory::FreeCreatureItem(user_creature::CCreatureItem* item)
{
    if (item->GetCreatureItemType() == 1)
        return m_creaturePool.Free(reinterpret_cast<user_creature::CCreature*>(item));
    if (item->GetCreatureItemType() == 0)
        return m_eggPool.Free(reinterpret_cast<user_creature::CEgg*>(item));
    return false;
}
