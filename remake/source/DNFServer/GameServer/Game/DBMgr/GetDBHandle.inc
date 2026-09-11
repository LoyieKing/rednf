MySQL* DBMgr::GetDBHandle(ENUM_DB_HANDLE_IDX idx, ENUM_SERVER_GROUP group)
{
    MySQL* base = m_dbHandle;
    if (!m_bActiveGlobalDB)
    {
        return &m_dbHandle[idx];
    }
    int fallbackIdx = 4;
    if (idx == 2 || idx == 3)
    {
        MySQL* p = GetDBHandleServerGroup(idx, group);
        if (p == 0)
        {
            return &base[4];
        }
        return p;
    }
    return &base[idx];
}