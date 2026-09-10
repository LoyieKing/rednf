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
