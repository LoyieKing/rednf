// ============================================================================
// _IsPunishedUser（ORIG T 0x084129d6）/ _isPunishedHackUser（ORIG T 0x08412ac8）
// ============================================================================

unsigned int DB_Login::_IsPunishedUser(unsigned int mid)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query("seLect count(*) from user_ban where m_id=%s and status=1",
                  NumberToString(mid, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 0;
    }
    int cnt = 0;
    if (!db->get_int(0, cnt) || cnt == 0)
    {
        return 0;
    }
    return 1;
}
