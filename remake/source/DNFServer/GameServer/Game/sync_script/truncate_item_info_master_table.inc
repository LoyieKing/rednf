bool CSyncScript::truncate_item_info_master_table()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    if (!db->set_query("trUncate table dnf_item_info_master"))
    {
        return false;
    }
    if (!db->exec(true))
    {
        return false;
    }
    return true;
