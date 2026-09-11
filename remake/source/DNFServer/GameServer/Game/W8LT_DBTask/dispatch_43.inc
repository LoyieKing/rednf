// ORIG 0x08427e54：GetDBHandle(1,0) → GetOutBuffer<SIG_UPDATE_USER_HANDICAP>；
// m_params[2](+0x10) 为 0 用永久 end_time，否则 adddate(now(),interval %d day)；
// upDate member_punish_info；exec 失败或 affected==0 时补 inSert；恒返 1。
bool DB_SaveUserHandicap::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    SIG_UPDATE_USER_HANDICAP* sig = stream->GetOutBuffer<SIG_UPDATE_USER_HANDICAP>();
    if (sig->m_params[2] == 0) {
        db->set_query(
            "upDate member_punish_info set occ_time=now(), start_time=now(), end_time='9999-12-31 23:59:59', punish_value=%d, apply_flag=2 where m_id=%s and punish_type=%d",
            sig->m_params[1], NumberToString(sig->m_accId, 0), sig->m_params[0]);
    } else {
        db->set_query(
            "upDate member_punish_info set occ_time=now(), start_time=now(), end_time=adddate(now(),interval %d day), punish_value=%d, apply_flag=2 where m_id=%s and punish_type=%d",
            sig->m_params[2], sig->m_params[1], NumberToString(sig->m_accId, 0),
            sig->m_params[0]);
    }
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0)) {
        if (sig->m_params[2] == 0) {
            db->set_query(
                "inSert into member_punish_info(m_id, occ_time, start_time, end_time, punish_type, punish_value, apply_flag) values(%s, now(), now(),'9999-12-31 23:59:59', %d, %d, 2)",
                NumberToString(sig->m_accId, 0), sig->m_params[0],
                sig->m_params[1]);
        } else {
            db->set_query(
                "inSert into member_punish_info(m_id, occ_time, start_time, end_time, punish_type, punish_value, apply_flag) values(%s, now(), now(), adddate(now(),interval %d day), %d, %d, 2)",
                NumberToString(sig->m_accId, 0), sig->m_params[2],
                sig->m_params[0], sig->m_params[1]);
        }
        db->exec(true);
    }
    return 1;
}