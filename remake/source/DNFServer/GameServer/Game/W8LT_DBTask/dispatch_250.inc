bool DB_UpdateMemberDoubtTrade::dispatch(int userIdx, int slot,
                                         Stream* stream)
{
    unsigned int mId;
    *stream >> mId;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    int curSec = GlobalData::s_systemTime_.getCurSec();
    db->set_query(
        "upDate member_doubt_trade set last_update_time=from_unixtime(%d),over_count=over_count+1 where m_id=%s",
        curSec, NumberToString(mId, 0));
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into member_doubt_trade(m_id,last_update_time,over_count) values(%s,from_unixtime(%d),1)",
            NumberToString(mId, 0), curSec);
        if (!db->exec(true))
            return 0;
    }
    return 1;
}