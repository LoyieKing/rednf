void cUserHistoryLog::SPSub(int treeKind, int remainSP, int sp, eSPSubReason reason)
{
    m_user->LogHistory("SP-,%d,%d,%d,%d", remainSP, sp, static_cast<int>(reason), treeKind);
