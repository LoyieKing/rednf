// ORIG C1 0x891b78e（0x8e）：两 string + vector<int> 构造后 clear()。
stPartyMatchingInfo::stPartyMatchingInfo()
    : m_channelName(), m_regionName(), m_dungeonIdx()
{
    clear();
}