DBMgr::~DBMgr()
{
    Close();
    delete[] m_dbHandle;
    for (std::map<ENUM_SERVER_GROUP, std::map<ENUM_DB_HANDLE_IDX, MySQL*> >::iterator outer =
             m_dbHandleMap.begin();
         outer != m_dbHandleMap.end(); ++outer)
    {
        for (std::map<ENUM_DB_HANDLE_IDX, MySQL*>::iterator inner = outer->second.begin();
             inner != outer->second.end(); ++inner)
        {
            MySQL* p = inner->second;
            if (p != 0)
            {
                delete p;
            }
        }
    }
