bool DB_UpdateBreakAwayUvList::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    int curSec = GlobalData::s_systemTime_.getCurSec();
    unsigned int mId = 0;
    *stream >> mId;
    SIG_BREAK_AWAY_UV_LIST* sig =
        stream->GetOutBuffer<SIG_BREAK_AWAY_UV_LIST>();
    db->set_query(
        "upDate churn_member_info set play_info ='%s', last_update_time=%u where m_id=%s",
        (char*)sig, curSec, NumberToString(mId, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
        return 0;
    return 1;
