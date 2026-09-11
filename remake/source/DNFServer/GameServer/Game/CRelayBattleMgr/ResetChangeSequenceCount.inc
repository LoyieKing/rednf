void CRelayBattleMgr::ResetChangeSequenceCount()
{
    for (int team = 1; team < 3; ++team)
    {
        m_changeSeqCount[team] = 0;
    }
}