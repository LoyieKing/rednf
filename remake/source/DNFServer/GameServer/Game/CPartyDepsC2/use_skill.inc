// ---------------------------------------------------------------------------
// use_skill —— ORIG T 0x085a77d8
// 报告：docs/class_func_reports/CParty/use_skill.md
// ---------------------------------------------------------------------------
void CParty::use_skill(CUser* user, int skillIdx, int param)
{
    OnUsedSkill(user, skillIdx, param);     // OnUsedSkill T 0x085b5f8e
}