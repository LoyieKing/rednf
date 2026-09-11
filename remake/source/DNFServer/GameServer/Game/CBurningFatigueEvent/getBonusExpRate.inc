int CBurningFatigueEvent::getBonusExpRate(unsigned short level) const
{
    // ORIG 0x810a0ec：从 i=1 开始找第一个 m_level > level 的项，返回其前一项
    // m_bonusExpRate(+0x04)；越界返回 0。每次访问均重解引用 this+0xc（ORIG 形态）。
    for (size_t i = 1; i < m_params->size(); ++i)
    {
        if (level < (*m_params)[i].m_level)
            return (*m_params)[i - 1].m_bonusExpRate;
    }
    return 0;
}