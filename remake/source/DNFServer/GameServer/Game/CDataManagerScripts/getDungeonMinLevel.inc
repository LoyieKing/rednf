// ORIG W 0x0822b514 _ZNK9CWorldMap18getDungeonMinLevelEv（const）：返回
// m_dungeonIndex（this+0x8）；StartGame::check_error 消费。
int CWorldMap::getDungeonMinLevel() const
{
    return m_dungeonIndex;
}