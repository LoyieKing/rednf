bool CSyncScript::truncate_exp_level_ref_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table exp_level_ref");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
}