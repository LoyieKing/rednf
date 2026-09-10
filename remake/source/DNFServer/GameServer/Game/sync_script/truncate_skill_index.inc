bool CSyncScript::truncate_skill_index()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table skill_index");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
