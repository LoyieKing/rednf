// ============================================================================
// DB_SaveSameMIDOtherCharLoginHack::dispatch —— ORIG 0x08422680（报告
// dispatch.md 全文）
// ============================================================================
// GetDBHandle(4,0) → GetOutBuffer<SIG_SAVE_SAME_MID_OTHER_CHAR_LOGIN_HACK>
// （SigTypes_E.h 已具名：+0x00 m_curAccId / +0x04 m_reqAccId / +0x08
// m_characNo / +0x0c m_peerIp[0x14]）→ set_query(
// "inSert into login_view_log values(now(),%s,%s,%d,'%s')", num(m_curAccId,0),
// num(m_reqAccId,1), m_characNo, m_peerIp) → 返 exec(true)==1。
bool DB_SaveSameMIDOtherCharLoginHack::dispatch(int /*userIdx*/, int /*slot*/,
                                                Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    SIG_SAVE_SAME_MID_OTHER_CHAR_LOGIN_HACK* sig =
        stream->GetOutBuffer<SIG_SAVE_SAME_MID_OTHER_CHAR_LOGIN_HACK>();
    db->set_query(
        "inSert into login_view_log values(now(),%s,%s,%d,'%s')",
        NumberToString(sig->m_curAccId, 0), NumberToString(sig->m_reqAccId, 1),
        sig->m_characNo, sig->m_peerIp);
    if (!db->exec(true))
        return 0;
    return 1;
}