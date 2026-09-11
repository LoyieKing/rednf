// ORIG T 0x0827abfa sz 0x80（SetMonsterInfo.md）：
void Secu_MonsterCheck::SetMonsterInfo(const MSG_MONSTER_DIE* msg)
{
    m_curKillCount = msg->m_mobSeq;      // +0x28 ← msg+0x0d
    m_curX = msg->m_killPairA24;         // +0x2a ← msg+0xa24
    m_curY = msg->m_killPairA26;         // +0x2c ← msg+0xa26
    m_setTick = GlobalData::s_systemTime_.getCurTickCount();   // +0x30
    init223(msg);
    init224(msg);
    init225(msg);
}