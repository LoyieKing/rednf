bool DB_UpdateBreakAwayClearAddInfo::dispatch(int userIdx, int slot,
                                              Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;
    *stream >> mId;
    db->set_query(
        "upDate churn_member_info set add_info=-1 where m_id=%s",
        NumberToString(mId, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        LogManager::logFormat(
            1, "DBThread.cpp",
            __PRETTY_FUNCTION__,
            0x85f4, "BREAK_AWAY : ADD INFO UPDATE ERROR!!");
        return 0;
    }
    return 1;
}