void CRelayBattleMgr::ArrangeMatchSequence()
{
    int team;
    int count;
    for (team = 1; team < 3; ++team)
    {
        int outIdx = 0;
        std::vector<int> seq(4u, 0);
        count = GetChangeSequenceCount(team);
        CreateRandomSequenceVector(seq, count);
        ArrangeChangeSequence(seq, team, outIdx);
        ArrangeNoChangeSequence(seq, team, outIdx);
    }
