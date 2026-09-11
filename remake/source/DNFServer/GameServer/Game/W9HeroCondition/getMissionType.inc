// ORIG getMissionType 0x08164fb6：mov 0xc(%eax) → this+0xc = condition+0x04，
// 即 m_field04（头注释原误标 +0x00，objdump 实证修正）。
unsigned int BaseHeroMissionCondition::getMissionType()
{
    return static_cast<unsigned int>(m_condition.m_field04);
}