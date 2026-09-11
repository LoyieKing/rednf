bool DB_UpdateBreakAwayAccruedCera::dispatch(int userIdx, int slot,
                                             Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;
    unsigned int cera = 0;
    *stream >> mId;
    *stream >> cera;
    db->set_query(
        "upDate churn_member_info set accrue_cera= accrue_cera+%u where m_id=%s",
        cera, NumberToString(mId, 0));
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0x8619, "BREAK_AWAY : UPDATE CERA ERROR!!");
        return 0;
    }
    return 1;
}