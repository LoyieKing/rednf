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
