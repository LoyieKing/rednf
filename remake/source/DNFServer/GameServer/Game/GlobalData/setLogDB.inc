void DBThread::setLogDB(bool logDB)
{
    // ORIG 0x083ff6c8：置标志后经成员回读；循环内经 isLogDB() 判定。
    m_bLogDB = logDB;
    m_queryCounter.setLogDB(m_bLogDB);
    for (int i = 0; i <= 0x10; ++i)
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)i,
                                                      (ENUM_SERVER_GROUP)0);
        if (isLogDB())
        {
            if (i == 4)
            {
                db->SetQueryCounter(&m_queryCounter);
            }
        }
        else if (i != 4)
        {
            db->SetQueryCounter(&m_queryCounter);
        }
    }
}