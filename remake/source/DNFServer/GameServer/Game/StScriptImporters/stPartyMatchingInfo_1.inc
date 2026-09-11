// ORIG C1ERKS_ 0x892ead6（0x86）：两 string + vector<int> 复制。
stPartyMatchingInfo::stPartyMatchingInfo(stPartyMatchingInfo const& other)
    : m_channelName(other.m_channelName),
      m_regionName(other.m_regionName),
      m_dungeonIdx(other.m_dungeonIdx)
{
}