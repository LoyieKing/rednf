// ============================================================================
// dispatch @ 0x08421f16
// 读 GetOutBuffer<SIG_SAVE_CLIENT_HACK_LOG>；GetDBHandle(DB_HANDLE_4, 0)。
// update：set charac_no=+0x04, cnt=cnt++0x0e, etc=+0x0a, where m_id=+0x00,
//   hack_type=+0x08；exec 成功且受影响行 !=0 → 特殊 hackType 处理
//   （0x193/0x1f7/0x326/0x3ba 命中 → _getTodaysHackCount + _sendHackTypeResult；
//   随后"todayCount != -1"恒假死代码段：_getTodaysHackCount >= todayCount →
//   DB_ReqSaveHackUserPunish::makeRequest(0xa8) + _sendResult）。
// 否则 insert 整行。恒返回 1。
// ============================================================================
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
}