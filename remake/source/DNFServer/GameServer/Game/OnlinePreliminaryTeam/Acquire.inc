// ORIG T 0x08588dd8：转调基类 StaticPool::Acquire（0x08588e56）。
COnlinePreliminaryTeam* CTeamFactory::Acquire()
{
    return StaticPool<COnlinePreliminaryTeam, 600>::Acquire();
}