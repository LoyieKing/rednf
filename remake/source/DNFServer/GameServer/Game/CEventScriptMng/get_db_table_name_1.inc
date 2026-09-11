// ORIG 0x810b110：按事件类型取实体，并转发到实体的表名访问器。
std::string& CEventScriptMng::get_db_table_name(int key)
{
    return get_event_entity(key)->get_db_table_name();
}