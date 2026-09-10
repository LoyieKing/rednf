void CParty::use_skill(CUser* user, int skillIdx, int param)
{
    OnUsedSkill(user, skillIdx, param);     // OnUsedSkill T 0x085b5f8e
