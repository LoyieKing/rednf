void cUserHistoryLog::KillMonster(int mobIndex, unsigned int killValue1,
                                  unsigned int killValue2, unsigned short a,
                                  unsigned short b, const MSG_MONSTER_DIE& msg,
                                  char roleType, bool isBoss, int curExp,
                                  int exp, eExpAddReason reason)
{
    m_user->LogHistory(
        "KillMob,%d,%d,%d, (%d:%d),%d, (%d:%d),%d,%d,%d, (%d:%d),%d,%d,%d,%d,%d,%d,%d",
        mobIndex, killValue1, killValue2,
        (unsigned)a & 0xffff, (unsigned)b & 0xffff,
        (unsigned)msg.m_fieldA22,
        (int)msg.m_fieldA29, (int)msg.m_fieldA2b,
        msg.m_fieldA31, msg.m_fieldA2d,
        (int)roleType,
        (int)msg.m_fieldA35, (int)msg.m_fieldA37,
        (int)msg.m_fieldA39, (int)msg.m_fieldA3b, (int)msg.m_fieldA3d,
        (unsigned)(isBoss ? 1 : 0),
        curExp, exp, (int)reason);
