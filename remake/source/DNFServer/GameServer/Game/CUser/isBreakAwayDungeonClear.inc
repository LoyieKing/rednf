bool CUser::isBreakAwayDungeonClear()
{
    return m_gameOption.m_breakAwayDungeonClear;   // ORIG movzbl 直返（无 test/setne）
