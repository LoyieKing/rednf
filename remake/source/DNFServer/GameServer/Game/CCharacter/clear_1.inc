void CCharacter::clear()
{
    // ORIG 0x0834852e..0x0834856c：base case 单条 + 循环 i≤5 写 m_baseInfoGrow[i]
    // （imul $0x52 +0x50/+0x2）——与 C1 混合构造形一致。
    m_baseInfo0.clear();
    for (int i = 0; i <= 5; ++i)
        m_baseInfoGrow[i].clear();
    for (int i = 0; i <= 6; ++i)
    {
        m_giveSkillList[i].clear();
        m_secondGrowTypes[i].clear();
    }
}