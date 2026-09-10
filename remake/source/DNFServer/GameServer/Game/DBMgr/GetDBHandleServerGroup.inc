MySQL* DBMgr::GetDBHandleServerGroup(ENUM_DB_HANDLE_IDX idx, ENUM_SERVER_GROUP group)
{
    if (group == 0)
    {
        __gnu_cxx::hash_map<int, stUserDBInfo_t>::const_iterator it =
            m_userDBInfoMap.find(m_userIdx);
        if (it == m_userDBInfoMap.end())
        {
            if (m_userIdx == -1)
            {
                return &m_dbHandle[idx];
            }
            return 0;
        }
        group = (ENUM_SERVER_GROUP)it->second.m_serverGroup;
    }
    return m_dbHandleMap[group][idx];
