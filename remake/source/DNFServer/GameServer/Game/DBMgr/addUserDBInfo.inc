void DBMgr::addUserDBInfo(int userIdx, const stUserDBInfo_t& info)
{
    std::pair<__gnu_cxx::hash_map<int, stUserDBInfo_t>::const_iterator, bool> ins =
        m_userDBInfoMap.insert(std::make_pair(userIdx, info));
    if (!ins.second)
    {
        m_userDBInfoMap[userIdx] = info;
    }
