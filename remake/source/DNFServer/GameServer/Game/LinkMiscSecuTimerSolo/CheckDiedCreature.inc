bool user_creature::CCreatureMgr::CheckDiedCreature()
{
    if (m_equippedCreature != 0)
        if (m_equippedCreature->CheckDiedCreature(m_user))
            return true;
    return false;
