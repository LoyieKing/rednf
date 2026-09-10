void cUserHistoryLog::InitSkill(int treeKind, int level, int sp, int sfp, eSkillInitReason reason)
{
    m_user->LogHistory("SkillInit,%d,%d,%d,%d,%d", treeKind, level, sp, sfp, static_cast<int>(reason));
