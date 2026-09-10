bool CQueryCounter::RegisterQuery(const char* query, int qid, int type)
{
    QUERY_INFO info;
    info.count = type;
    std::pair<std::map<int, QUERY_INFO>::iterator, bool> ins =
        m_queryInfoMap.insert(std::make_pair(qid, info));
    if (!ins.second)
    {
        return false;
    }
    std::pair<std::map<std::string, std::map<int, QUERY_INFO>::iterator>::iterator, bool> ins2 =
        m_queryIDMap.insert(std::make_pair(query, ins.first));
    if (!ins2.second)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x291, 0);
        trace("LoadQueryTable,  query_id_map_.insert ERROR");
        return false;
    }
    return true;
