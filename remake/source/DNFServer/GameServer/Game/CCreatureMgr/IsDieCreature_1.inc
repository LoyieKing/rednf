// ORIG IsDieCreature @ 0x833a660：空返回 1，否则委托 CCreature::IsDieCreature。
bool CCreatureMgr::IsDieCreature() const
{
    // ORIG 0x833a66e：test;je→mov1 尾块；call fall-through = 三元 ?: 形态。
    return m_equippedCreature != 0 ? m_equippedCreature->IsDieCreature() : true;
}
