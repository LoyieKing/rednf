// ============================================================================
// DB_SaveProxyLog::dispatch —— ORIG 0x08422742
// ============================================================================
// ORIG 0x8422742：GetOutBuffer<SIG_SAVE_PROXY_LOG>() →
// GetDBHandle(DB_HANDLE_4,0) → set_query("inSert into dnf_proxy_log
// values(now(),%s,'%s','%s',%d)", NumberToString(accId,0), m_strIp,
// m_peerIp, (uint)(byte)m_natType) → exec(true) → return exec==1。
bool DB_SaveProxyLog::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_SAVE_PROXY_LOG* sig = stream->GetOutBuffer<SIG_SAVE_PROXY_LOG>();
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_4,
                                                  E_SERVER_GROUP_NONE);
    db->set_query(
        "inSert into dnf_proxy_log values(now(),%s,'%s','%s',%d)",
        NumberToString(sig->m_accId, 0), sig->m_strIp, sig->m_peerIp,
        (unsigned int)sig->m_natType);
    if (!db->exec(true))
        return 0;
    return 1;
}