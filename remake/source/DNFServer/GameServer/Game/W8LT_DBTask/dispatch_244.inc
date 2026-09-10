bool DB_UpdateBreakAwayDungeonClearCnt::dispatch(int userIdx, int slot,
                                                 Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mId = 0;
    int clearCnt = 0;
    *stream >> mId;
    *stream >> clearCnt;
    db->set_query(
        "upDate churn_member_info set add_info=%d where m_id=%s",
        clearCnt, NumberToString(mId, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
        return 0;
    return 1;
