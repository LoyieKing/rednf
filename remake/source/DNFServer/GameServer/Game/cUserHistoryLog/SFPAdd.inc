void cUserHistoryLog::SFPAdd(int treeKind, int remainSFP, int sfp, eSPAddReason reason)
{
    m_user->LogHistory("SFP+,%d,%d,%d,%d", remainSFP, sfp, static_cast<int>(reason), treeKind);
}