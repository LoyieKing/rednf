// ===== _ZN35FAIR_PVP_SKILL_TREE_SKILL_PER_POINT5clearEv —— ORIG 0x08a5e874 =====
// 反汇编：map@+0x00 clear、map@+0x18 clear、vector<pair>@+0x30 clear、int@+0x3c=0。
void FAIR_PVP_SKILL_TREE_SKILL_PER_POINT::clear()
{
    m_addSkills.clear();
    m_removeSkills.clear();
    m_giveSkills.clear();
    m_pointCost = 0;
}
