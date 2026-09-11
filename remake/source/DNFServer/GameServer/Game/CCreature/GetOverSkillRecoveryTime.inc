// ORIG _ZN13user_creature9CCreature24GetOverSkillRecoveryTimeEv T 0x8338c50。
int CCreature::GetOverSkillRecoveryTime()
{
    if (m_script == 0)
        return -1;
    return m_script->GetOverSkillRecoveryTime();
}