// ============================================================================
// DB_SecuServiceReward::dispatch —— ORIG 0x08437764（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x8437764：GetDBHandle(DB_HANDLE_1,0) → GetOutBuffer<
// SIG_SECU_SERVICE_REWARD>（+0 m_id）→ upDate member_security_grade set
// last_check_time=now() where m_id=%s（%s=NumberToString(m_id,0)）→
// exec(true)==1 且 getAffectedRowCount()!=0 → 返 1；否则
// cMyTrace(0x8461,0)("DB_SecuServiceReward Update Error") → 返 0。
// ============================================================================
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
}