void cUserHistoryLog::SFPSub(int treeKind, int remainSFP, int sfp, eSPSubReason reason)
{
    m_user->LogHistory("SFP-,%d,%d,%d,%d", remainSFP, sfp, static_cast<int>(reason), treeKind);
