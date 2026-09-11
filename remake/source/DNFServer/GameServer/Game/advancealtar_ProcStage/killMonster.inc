// ----------------------------------------------------------------------------
// ORIG T 0x08133b40（killMonster.md 全文）：
//   cUserHistoryLog(user+0x79700).KillMonster(monsterId,
//     msg.m_killValue1(+0xa1e), msg.m_killValue2(+0xa1a),
//     msg.m_killPairA24(+0xa24), msg.m_killPairA26(+0xa26),
//     msg, roleType=0, isBoss=false, curExp=0, exp=0, eExpAddReason=1)。
// ----------------------------------------------------------------------------
void HistoryLog::killMonster(CUser& user, int monsterId, const MSG_MONSTER_DIE& msg)
{
    user.m_historyLog.KillMonster(monsterId,
                                  msg.m_killValue1,
                                  msg.m_killValue2,
                                  msg.m_killPairA24,
                                  msg.m_killPairA26,
                                  msg, (char)0, false, 0, 0,
                                  (eExpAddReason)1);
}