int CCreatureMgr::GetEquipedCreatureLevel() const
{
    // ORIG 0x833a6be：test;je→mov0 尾块；call fall-through = 三元 ?: 形态。
    return m_equippedCreature != 0 ? m_equippedCreature->GetLevel() : 0;
}
