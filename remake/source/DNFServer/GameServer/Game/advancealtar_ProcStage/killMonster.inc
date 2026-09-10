void HistoryLog::killMonster(CUser& user, int monsterId, const MSG_MONSTER_DIE& msg)
{
    user.m_historyLog.KillMonster(monsterId,
                                  msg.m_killValue1,
                                  msg.m_killValue2,
                                  msg.m_killPairA24,
                                  msg.m_killPairA26,
                                  msg, (char)0, false, 0, 0,
                                  (eExpAddReason)1);
