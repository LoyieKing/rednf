bool CCreatureMgr::VerifyEquipedCreatureQuest() const
{
    // ORIG 0x833d708：je→尾mov0；then(IsAbleSelectEvolute)=fall，else=je 目标。
    if (m_equippedCreature != 0)
    {
        if (m_equippedCreature->IsGrowCreature())
            return m_equippedCreature->IsAbleSelectEvolute(m_user);
        else
            return m_equippedCreature->IsAbleEvolute(m_user);
    }
    return 0;
