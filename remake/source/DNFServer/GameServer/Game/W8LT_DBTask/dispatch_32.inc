bool DB_UpdateLogin::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_UPDATE_LOGIN* sig = stream->GetOutBuffer<SIG_UPDATE_LOGIN>();
    if (sig == 0)
        return 0;
    int tableNo = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        tableNo = ((DB_UpdateLoginView*)sig)->m_serverGroup;
    else
        tableNo = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query(
        "upDate login_account_%d set m_channel_no=%d,login_status=1,last_login_date=now(),login_ip='%s' where m_id=%s",
        tableNo, ((DB_UpdateLoginView*)sig)->m_channelNo,
        ((DB_UpdateLoginView*)sig)->m_loginIp,
        NumberToString(((DB_UpdateLoginView*)sig)->m_accId, 0));
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into login_account_%d (m_id,m_channel_no,login_status,last_login_date,login_ip) values(%s,%d,1,now(),'%s')",
            tableNo, NumberToString(((DB_UpdateLoginView*)sig)->m_accId, 0),
            ((DB_UpdateLoginView*)sig)->m_channelNo,
            ((DB_UpdateLoginView*)sig)->m_loginIp);
        if (!db->exec(true))
            return 0;
    }
    return 1;
