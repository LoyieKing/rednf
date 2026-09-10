int CCreatureMgr::ItemEvolute(int itemIdx)
{
    if (m_equippedCreature != 0)
        return m_equippedCreature->ItemEvolute(m_user, itemIdx);
    return 0;
