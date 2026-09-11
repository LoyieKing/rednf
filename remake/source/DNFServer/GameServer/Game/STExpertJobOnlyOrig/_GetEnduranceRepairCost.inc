STExpertJobScript::_GetEnduranceRepairCost(unsigned int grade)
{
    if (m_enduranceRepairCost.size() <= grade - 1u)  // 无符：grade=0 → 0xffffffff 恒真
        return 0;
    stEnduranceRepairCost* c = &m_enduranceRepairCost.at(grade - 1u);
    return c;
}