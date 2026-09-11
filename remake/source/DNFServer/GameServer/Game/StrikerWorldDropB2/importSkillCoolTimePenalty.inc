// ORIG T 0x08a9e5fc：clear this+0x58 后直呼 readSkillPenalty(this+0x58)。
void StrikerScript::importSkillCoolTimePenalty()
{
    m_coolTimePenalty.clear();
    readSkillPenalty(m_coolTimePenalty);
}