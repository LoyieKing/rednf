// ORIG 0x842b6e6：GetDBHandle(DB_HANDLE_6,0) →
// GetOutBuffer<SIG_SECURITY_CARD_DISUSE> → turnOffSecurityFlag(db, *(uint*)sig)
// → set_query("upDate member_security_card set apply_flag=0 where m_id=%s",
// NumberToString(m_id,0)) → exec(true)（结果丢弃）→
// saveSecurityCardHistory(db, m_id, ENUM_SECURITYCARD_MOD_TYPE_DISUSE=2)
// → 返 1。
// ============================================================================
bool DB_SecurityCardDisuse::dispatch(int /*userIdx*/, int /*slot*/,
                                     Stream* stream)
{
    MySQL* db;                             // -0x10
    SIG_SECURITY_CARD_DISUSE* sig;         // -0xc
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                           E_SERVER_GROUP_NONE);
    sig = stream->GetOutBuffer<SIG_SECURITY_CARD_DISUSE>();
    turnOffSecurityFlag(db, ((SecuDisuseView*)sig)->m_id);
    db->set_query(
        "upDate member_security_card set apply_flag=0 where m_id=%s",
        NumberToString(((SecuDisuseView*)sig)->m_id, 0));
    db->exec(true);
    saveSecurityCardHistory(db, ((SecuDisuseView*)sig)->m_id,
                            ENUM_SECURITYCARD_MOD_TYPE_DISUSE);
    return 1;
}