void CClientCrashDownLogStatistic::MakeClientCrashDownQuery(
    std::vector<std::string>& queries, int queryPrefixLength)
{
    std::string values;
    for (unsigned int i = 0; i < m_logRecords.size(); ++i)
    {
        values.clear();
        for (std::map<int, int>::iterator it = m_logRecords[i].m_reasonCount.begin(),
                                          end = m_logRecords[i].m_reasonCount.end();
             it != end; )
        {
            if (queryPrefixLength <= 0x6000 - static_cast<int>(values.size()))
            {
                if (!values.empty()) values += ",";
                char value[0xff] = { '\0' };
                std::sprintf(value, "(from_unixtime(%d),%d,%d,%d)",
                             m_logRecords[i].m_occTime,
                             G_CEnvironment()->get_channel_no(), it->first,
                             it->second);
                values += value;
                ++it;
            }
            else
                break;
        }
        if (!values.empty()) queries.push_back(values);
    }
}