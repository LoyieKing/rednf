bool DB_SavePremiumInfo::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_SAVE_PREMIUM_INFO* raw =
        stream->GetOutBuffer<SIG_SAVE_PREMIUM_INFO>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query(
        "upDate member_premium set service_end=from_unixtime(%d) where event_id=%d and pre_type=%d and m_id=%s and service_end>from_unixtime(%d) and server_id=%d",
        ((DB_SavePremiumInfoView*)raw)->m_serviceEnd,
        ((DB_SavePremiumInfoView*)raw)->m_eventId,
        ((DB_SavePremiumInfoView*)raw)->m_preType,
        NumberToString(((DB_SavePremiumInfoView*)raw)->m_mId, 0),
        ((DB_SavePremiumInfoView*)raw)->m_serviceStart,
        ((DB_SavePremiumInfoView*)raw)->m_serverId);
    if (!(db->exec(true) && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into member_premium(pre_type,m_id,service_start,service_end,event_id,server_id) values(%d,%s,from_unixtime(%d),from_unixtime(%d),%d,%d)",
            ((DB_SavePremiumInfoView*)raw)->m_preType,
            NumberToString(((DB_SavePremiumInfoView*)raw)->m_mId, 0),
            ((DB_SavePremiumInfoView*)raw)->m_serviceStart,
            ((DB_SavePremiumInfoView*)raw)->m_serviceEnd,
            ((DB_SavePremiumInfoView*)raw)->m_eventId,
            ((DB_SavePremiumInfoView*)raw)->m_serverId);
        if (!db->exec(true))
            return 0;
    }
    return 1;
