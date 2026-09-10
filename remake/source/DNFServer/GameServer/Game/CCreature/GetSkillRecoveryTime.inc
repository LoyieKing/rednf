int CCreature::GetSkillRecoveryTime()
{
    if (m_script == 0)
        return -1;
    return m_script->GetSkillRecoveryTime();
