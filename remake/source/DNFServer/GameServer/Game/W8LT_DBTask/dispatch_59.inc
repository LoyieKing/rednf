bool DB_SaveClientHackLog::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_SAVE_CLIENT_HACK_LOG_VIEW* v = reinterpret_cast<SIG_SAVE_CLIENT_HACK_LOG_VIEW*>(
        stream->GetOutBuffer<SIG_SAVE_CLIENT_HACK_LOG>());
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        DB_HANDLE_4, static_cast<ENUM_SERVER_GROUP>(0));

    db->set_query("upDate dnf_hack_log set charac_no=%d,last_time=now(),cnt=cnt+%d,"
                  "etc=%d where m_id=%s and hack_type=%d and "
                  "occ_date=CAST(DATE_FORMAT(now(), '%%Y-%%m-%%d') AS CHAR)",
                  v->m_characNo,
                  v->m_cnt,
                  v->m_etc,
                  NumberToString(v->m_mid, 0),
                  static_cast<int>(v->m_hackType));

    if (!db->exec(true) || db->getAffectedRowCount() == 0)
    {
        db->set_query("inSert into dnf_hack_log(m_id,charac_no,hack_type,occ_date,"
                      "last_time,etc,cnt) values(%s,%d,%d,cast(now() as date),"
                      "now(),%d,%d)",
                      NumberToString(v->m_mid, 0),
                      v->m_characNo,
                      static_cast<int>(v->m_hackType),
                      v->m_etc,
                      v->m_cnt);
        db->exec(true);
    }
    else
    {
        int todayCount = -1;
        switch (v->m_hackType)
        {
        case 0x193:
        case 0x1f7:
        case 0x326:
        case 0x3ba:
            _sendHackTypeResult(v->m_mid, v->m_hackType,
                                _getTodaysHackCount(db, v->m_mid, v->m_hackType));
            break;
        default:
            break;
        }
        if (todayCount != -1)
        {
            if (_getTodaysHackCount(db, v->m_mid, v->m_hackType) >= todayCount)
            {
                DB_ReqSaveHackUserPunish::makeRequest(v->m_mid, 0xa8, 0);
                _sendResult(v->m_mid);
            }
        }
    }
    return 1;
