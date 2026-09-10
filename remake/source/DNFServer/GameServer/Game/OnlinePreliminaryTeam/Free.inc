void CTeamFactory::Free(COnlinePreliminaryTeam* team)
{
    StaticPool<COnlinePreliminaryTeam, 600>::Free(team);
