void CUser::setBreakAwayRewardOrder(ENUM_BREAK_AWAY_REWARD_ORDER order)
{
    // ORIG 0x0831d4de：enum=enum 同型赋值 this 先载（CGameOption.h 字段已具型）
    m_gameOption.m_breakAwayRewardOrder = order;
