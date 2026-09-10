void cUserHistoryLog::SkillDel(int job, int treeKind, int skillIdx, int count, eSkillDelReason reason)
{
    m_user->LogHistory("Skill-,%d,%d,%d,%d,%d", job, skillIdx, count, static_cast<int>(reason), treeKind);
