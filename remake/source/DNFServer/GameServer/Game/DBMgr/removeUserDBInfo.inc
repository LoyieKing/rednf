void DBMgr::removeUserDBInfo(int userIdx)
{
    if (GlobalData::s_db_mgr->isActiveGlobalDB() != true)
    {
        return;
    }
    m_userDBInfoMap.erase(userIdx);
