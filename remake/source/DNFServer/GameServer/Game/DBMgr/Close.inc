void DBMgr::Close()
{
    for (int i = 0; i < 0x11; i = i + 1)
    {
        m_dbHandle[i].close();
    }
