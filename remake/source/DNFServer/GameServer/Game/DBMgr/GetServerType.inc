int DBMgr::GetServerType(int userIdx)
{
    __gnu_cxx::hash_map<int, stUserDBInfo_t>::const_iterator it =
        m_userDBInfoMap.find(userIdx);
    if (it != m_userDBInfoMap.end())
    {
        return it->second.m_serverGroup;
    }
    return 0;
