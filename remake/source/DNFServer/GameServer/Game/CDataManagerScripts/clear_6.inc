void stRankSystemInfo::stDungeonRankSystem_t::clear()
{
    // ORIG 0x8370e34：清 map/vector + memset +0x24 0x12 字节
    m_map0.clear();
    m_vec18.clear();
    // ORIG 0x8370e34：+0x24 起 memset 0x12 字节 = m_rankLevel[9] 全体
    memset(m_rankLevel, 0, sizeof(m_rankLevel));
}