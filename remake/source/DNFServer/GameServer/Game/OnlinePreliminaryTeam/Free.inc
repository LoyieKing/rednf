// ORIG T 0x08588dec：转调基类 StaticPool::Free。
void CTeamFactory::Free(COnlinePreliminaryTeam* team)
{
    StaticPool<COnlinePreliminaryTeam, 600>::Free(team);
}