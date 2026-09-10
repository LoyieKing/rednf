bool CCreatureMgr::IsGrowCreature_Equipped_Creature() const
{
    // ORIG 0x833debc：je→尾mov0；call;test al;je→中mov0;jmp；fall mov1。
    if (m_equippedCreature != 0)
    {
        if (m_equippedCreature->IsGrowCreature())
            return true;
        return false;
    }
    return false;
