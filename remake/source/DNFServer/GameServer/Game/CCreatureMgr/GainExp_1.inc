bool CCreatureMgr::GainExp(int exp)
{
    // ORIG 0x8339b14/0x8339b37：两条短路均 je 直达尾 mov0 岛（无 xor 取反）；
    // 两条 false 路径共用单一尾 return（ORIG 两 je 同指 0x8339b66）。
    if (m_equippedCreature != 0)
    {
        if (m_equippedCreature->GainExp(exp, m_user))
        {
            if (m_equippedCreature->IsGrowCreature())
                m_isMaxLevelGrowCreature =
                    m_equippedCreature->isMaxLevel() ? 1 : 0;
            return true;
        }
    }
    return false;
