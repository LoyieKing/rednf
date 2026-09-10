std::string& CEventEntity::get_db_table_name()
{
    // ORIG 0x810b06c：return &m_dbTableName（+0x08）。
    return m_dbTableName;
