// ORIG _ZN13user_creature9CCreature22GetLearnOverSkillLevelEv T 0x8338c78。
int CCreature::GetLearnOverSkillLevel()
{
    if (m_script == 0)
        return -1;
    return m_script->GetLearnOverSkillLevel();
}