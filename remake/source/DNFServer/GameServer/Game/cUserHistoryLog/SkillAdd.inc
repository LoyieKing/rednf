void cUserHistoryLog::SkillAdd(int job, int treeKind, int skillIdx, int count, eSkillAddReason reason)
{
    m_user->LogHistory("Skill+,%d,%d,%d,%d,%d", job, skillIdx, count, static_cast<int>(reason), treeKind);
