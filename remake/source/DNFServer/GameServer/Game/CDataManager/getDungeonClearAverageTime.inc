// ORIG T 0x083639d2 _ZN12CDataManager26getDungeonClearAverageTimeEii23ENUM_DUNGEON_DIFFICULTY：
//   以 (dungeonIdx, partyCount) 为键查 m_rankSystem.m_dungeonRank.m_map0；
//   未命中返回 0；命中按 difficulty(0..3) 取 stClearTimeAverage::m_v[难度]，
//   其它难度返回 0。
unsigned int CDataManager::getDungeonClearAverageTime(int dungeonIdx, int partyCount,
