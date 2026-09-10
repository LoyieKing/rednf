bool CSyncScript::truncate_charac_quest_ref()
{
    if (m_field0 != 1)
    {
        return true;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    db->set_query("trUncate table charac_quest_ref; ");
    if (!db->exec(true))
    {
        return false;
    }
    return true;
