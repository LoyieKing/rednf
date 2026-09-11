int DB_Login::GetDailyCleanPadPoint(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query("seLect cleanpad_point from member_login where m_id=%s",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true) || db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 0;
    }
    if (!db->get_uint(0, reinterpret_cast<unsigned int&>(sig->m_cleanPadDecreaseCnt)))  // +0x3a4c
    {
        return 0;
    }
    return 1;
}