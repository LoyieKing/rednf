bool DB_SecuServiceReward::dispatch(int /*userIdx*/, int /*slot*/,
                                    Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  E_SERVER_GROUP_NONE);
    SIG_SECU_SERVICE_REWARD* sig =
        stream->GetOutBuffer<SIG_SECU_SERVICE_REWARD>();
    db->set_query(
        "upDate member_security_grade set last_check_time=now() where m_id=%s",
        NumberToString(sig->m_mId, 0));
    if (db->exec(true) != 1 || db->getAffectedRowCount() == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x8461, 0);
        trace("DB_SecuServiceReward Update Error");
        return 0;
    }
    return 1;
