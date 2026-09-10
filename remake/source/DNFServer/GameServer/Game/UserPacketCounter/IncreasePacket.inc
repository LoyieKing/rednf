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
