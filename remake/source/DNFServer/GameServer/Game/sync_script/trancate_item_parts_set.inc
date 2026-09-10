bool CSyncScript::trancate_item_parts_set()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)5, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table item_part_set");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
