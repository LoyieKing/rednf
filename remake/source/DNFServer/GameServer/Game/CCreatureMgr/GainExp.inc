bool CCreature::GainExp(int exp, CUser* user)
{
    if (m_script == 0)
        return false;
    if (m_script->GetTypeSelectEvolutionLevel() != -1)
    {
        if (m_level >= m_script->GetTypeSelectEvolutionLevel())
            return false;
    }
    if (m_level >= m_script->GetMaxLevel())
        return false;
    // ORIG 0x833797e：movzbl;xor$1;test;je <GainExpCreature>，fall=GainItemExp
    // （P194 X1 块序：xor 物化 (m_isGrow!=1)，je 是 ==1 成功分支）。
    if (m_isGrow != 1)
        return GainItemExpCreature(exp, user);
    return GainExpCreature(exp, user);
}
