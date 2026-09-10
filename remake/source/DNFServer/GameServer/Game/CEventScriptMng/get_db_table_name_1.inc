std::string& CEventScriptMng::get_db_table_name(int key)
{
    return get_event_entity(key)->get_db_table_name();
