void cUserHistoryLog::SPAdd(int treeKind, int remainSP, int sp, eSPAddReason reason)
{
    m_user->LogHistory("SP+,%d,%d,%d,%d", remainSP, sp, static_cast<int>(reason), treeKind);
