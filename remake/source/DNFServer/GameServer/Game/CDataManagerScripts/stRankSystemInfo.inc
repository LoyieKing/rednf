// ORIG C1 0x8370eba：构造子对象 + 4 容器后调 clear()（clear 内写 +0x74）。
stRankSystemInfo::stRankSystemInfo() : m_dungeonRank(), m_vec38(), m_map44(),
                                       m_vec5c(), m_vec68()
{
    clear();
}