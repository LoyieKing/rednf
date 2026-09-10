void CPowerWarPacketLog::Reset()
{
    for (std::map<int, STPacketLog*>::iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        STPacketLog* log = it->second;
        if (log != 0)
            delete log;
    }
    m_map.clear();
