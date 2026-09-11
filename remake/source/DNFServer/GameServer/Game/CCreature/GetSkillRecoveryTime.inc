// ORIG _ZN13user_creature9CCreature20GetSkillRecoveryTimeEv T 0x8338c28。
int CCreature::GetSkillRecoveryTime()
{
    if (m_script == 0)
        return -1;
    return m_script->GetSkillRecoveryTime();
}