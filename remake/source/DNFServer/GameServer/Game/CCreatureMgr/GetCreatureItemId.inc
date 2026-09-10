int CCreatureMgr::GetCreatureItemId() const
{
    // ORIG 0x833a638：test;je→mov0 尾块；call fall-through = 三元 ?: 形态。
    return m_equippedCreature != 0
               ? reinterpret_cast<CCreatureItem*>(m_equippedCreature)->GetItemId()
               : 0;
}
