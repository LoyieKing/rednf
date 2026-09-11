// ORIG GainExp @ 0x83378fc：路由入口（reports/user_creature__CCreature/GainExp.md）。
// m_script==0 → 0；选择进化等级有效（!=-1）且 m_level>=它 → 0；
// m_level<GetMaxLevel 时按 m_isGrow 分流：==1 走 GainExpCreature，否则 GainItemExpCreature；
// 达到 MaxLevel → 0。
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
