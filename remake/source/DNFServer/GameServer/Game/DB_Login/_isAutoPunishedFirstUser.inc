bool DB_Login::_isAutoPunishedFirstUser(unsigned int mid)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query("seLect punish_flag from auto_punish_first_user where m_id=%s and punish_flag=1",
                  NumberToString(mid, 0));
    // ORIG 0x8411085：exec 失败或 rows==0 → 聚合 bad → return false
    if (!db->exec(true) || db->get_n_rows() == 0)
    {
        return false;
    }
    return true;
}