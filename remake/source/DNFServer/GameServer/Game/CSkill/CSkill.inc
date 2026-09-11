// ---- 构造 / 析构（ORIG 0x08374944 / 0x08379fd8）----
CSkill::CSkill()
{
    memset(m_fitnessSecondGrowType, 0, sizeof(m_fitnessSecondGrowType));
    m_isSpecialSkill = 0;
    m_keyCommandList.clear();
    m_isCommandCustomizing = 1;
    memset(m_skillName, 0, sizeof(m_skillName));
    for (int i = 0; i <= 5; ++i)
    {
        m_fitGrowType[i] = -1;
    }
}