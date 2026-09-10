bool CCreatureMgr::Dismantle(CCreature* creature)
{
    if (reinterpret_cast<CCreatureItem*>(creature)->GetCreatureItemType() != 1)
        return false;
    if (!creature->Dismantle(m_user))
        return false;
    m_equippedCreature = 0;
    return true;
