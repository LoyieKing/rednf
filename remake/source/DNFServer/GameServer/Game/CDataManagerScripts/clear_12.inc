// ORIG clear 0x837105a：map.clear()、vector.clear()、+0x24/+0x28/+0x2c 三 int 清零。
void stItemMakingSkill::clear()
{
    m_map0.clear();
    m_vec18.clear();
    m_exp = 0;
    m_bonusExp = 0;
    m_levelLimit = 0;
}