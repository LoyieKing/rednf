COnlinePreliminaryTeam* CTeamFactory::Acquire()
{
    return StaticPool<COnlinePreliminaryTeam, 600>::Acquire();
