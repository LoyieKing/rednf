const char* CCreatureMgr::GetCreatureName() const
{
    // ORIG 0x833a693：test;je→mov$<A> 尾块；call fall-through = 三元 ?: 形态。
    return m_equippedCreature != 0 ? m_equippedCreature->GetName()
                                   : "";  // ORIG &DAT_08c2d760 空串
