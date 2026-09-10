void CBossTower::_destroy()
{
    m_dungeonLog.BossTowerLogDBUpdate();
    m_dungeonLog.reset();
    G_CGameManager()->returnBossTower(this);
