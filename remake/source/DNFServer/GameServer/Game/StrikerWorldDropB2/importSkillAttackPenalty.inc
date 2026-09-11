// ORIG T 0x08a9e5d0：clear this+0x4c 后直呼 readSkillPenalty(this+0x4c)。
void StrikerScript::importSkillAttackPenalty()
{
    m_attackPenalty.clear();
    readSkillPenalty(m_attackPenalty);
}