bool DBMgr::Open(ENUM_DB_HANDLE_IDX idx, char* host, unsigned int port,
                 char* db, char* user, char* passwd, ENUM_SERVER_GROUP group)
{
    if (group != 0)
    {
        if (m_dbHandleMap[group][idx] != 0)
        {
            return false;
        }
        m_dbHandleMap[group][idx] = new MySQL;
        if (!m_dbHandleMap[group][idx]->init())
        {
            return false;
        }
        if (!m_dbHandleMap[group][idx]->open(host, port, db, user, passwd))
        {
            return false;
        }
        return true;
    }
    bool ret = m_dbHandle[idx].open(host, port, db, user, passwd);
    return ret;
}