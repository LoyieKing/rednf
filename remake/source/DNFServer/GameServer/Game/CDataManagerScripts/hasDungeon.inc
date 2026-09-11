// ORIG W 0x822b52c _ZNK9CWorldMap10hasDungeonEj（0x85 字节）：
// std::find(m_dungeonList.begin(), m_dungeonList.end(), dungeonIdx) != end()。
// 实参为 unsigned int（find 模板 _ZSt4findI...jE...）；调用方
// DisPatcher_SelectDungeon::process 传 (int)(short)MSG_CHARAC_SELECT+0xd。
bool CWorldMap::hasDungeon(unsigned int dungeonIdx) const
{
    return std::find(m_dungeonList.begin(), m_dungeonList.end(), dungeonIdx) !=
           m_dungeonList.end();
}