void DBMgr::Close(ENUM_DB_HANDLE_IDX idx)
{
    if (idx >= 0 && idx < 0x11)
    {
        m_dbHandle[idx].close();
    }
