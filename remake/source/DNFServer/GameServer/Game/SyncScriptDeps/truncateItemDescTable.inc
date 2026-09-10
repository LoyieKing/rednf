bool SyncScript::truncateItemDescTable()
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table charac_advance_altar_item_desc");
    if (!db->exec(true))
        return false;
    return true;
