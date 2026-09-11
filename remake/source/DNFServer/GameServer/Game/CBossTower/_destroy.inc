// ORIG T 0x08142b36：
//   m_dungeonLog.BossTowerLogDBUpdate() → m_dungeonLog.reset() →
//   G_CGameManager()->returnBossTower(this)。
void CBossTower::_destroy()
{
    m_dungeonLog.BossTowerLogDBUpdate();
    m_dungeonLog.reset();
    G_CGameManager()->returnBossTower(this);
}