// ORIG T 0x0856ff24：
//   find(accId) != end → ++it->second；否则 insert(make_pair(accId, 1))；
//   最后 ++m_totalCount（+0x118）。
void CUserPacketCounter::IncreasePacket(unsigned int accId)
{
    std::map<unsigned int, int>::iterator it = m_counter.find(accId);
    if (it != m_counter.end())
    {
        ++it->second;
    }
    else
    {
        m_counter.insert(std::make_pair(accId, 1));
    }
    ++m_totalCount;
}