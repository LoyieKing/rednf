// ORIG _ZNK13user_creature12CCreatureMgr26VerifyEquipedCreatureQuestEv
// T 0x0833d6fa：无装备宠物 → 0；成长宠走 IsAbleSelectEvolute，否则 IsAbleEvolute。
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
}
