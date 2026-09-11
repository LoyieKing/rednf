// ============================================================================
// DB_GoblinPadUpdateValidityTime::dispatch —— ORIG 0x0842f22a（报告 dispatch.md 全文）
// ============================================================================
// ORIG 0x842f22a：GetDBHandle(DB_HANDLE_6,0) → GetOutBuffer<
// SIG_GOBLIN_UPDATE_VALIDITYTIME>（+0 m_id / +4 flag）→ upDate
// member_mousepass set validity_time=unix_timestamp(now())+%d where m_id=%s
// （%d=flag、%s=NumberToString(m_id,0)）→ exec(true)（返回值丢弃）→ 恒返 1。
// ============================================================================
bool DB_GoblinPadUpdateValidityTime::dispatch(int /*userIdx*/, int /*slot*/,
                                              Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  E_SERVER_GROUP_NONE);
    SIG_GOBLIN_UPDATE_VALIDITYTIME* sig =
        stream->GetOutBuffer<SIG_GOBLIN_UPDATE_VALIDITYTIME>();
    db->set_query(
        "upDate member_mousepass set validity_time=unix_timestamp(now())+%d where m_id=%s",
        sig->m_flag,
        NumberToString(sig->m_mId, 0));
    db->exec(true);
    return 1;
}