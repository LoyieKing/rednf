std::string& CObjectBringUp::get_db_table_name()
{
    // ORIG 0x0810fcdc：return &m_dbTableName（ObjectBringUpScript +0x08）。
    return static_cast<ObjectBringUpScript*>(m_script1c)->m_dbTableName;
}