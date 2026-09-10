bool CCreatureMgr::Equip(CCreature* creature)
{
    // ORIG 经外联 GetCreatureItemType(W 0x833ef98) 取类型后 !=1 判定（无中间局部）
    if (reinterpret_cast<CCreatureItem*>(creature)->GetCreatureItemType() != 1)
        return false;
    if (!creature->Equip(m_user))
        return false;
    m_equippedCreature = creature;
    return true;
}
