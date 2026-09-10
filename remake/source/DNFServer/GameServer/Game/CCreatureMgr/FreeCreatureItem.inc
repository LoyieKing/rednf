bool user_creature::CCreatureItemFactory::FreeCreatureItem(user_creature::CCreatureItem* item)
{
    if (item->GetCreatureItemType() == 1)
        return m_creaturePool.Free(reinterpret_cast<user_creature::CCreature*>(item));
    if (item->GetCreatureItemType() == 0)
        return m_eggPool.Free(reinterpret_cast<user_creature::CEgg*>(item));
    return false;
}
