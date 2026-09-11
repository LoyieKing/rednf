std::string& CAddItemCountDB::get_db_table_name()
{
    // ORIG 0x810e15a：return &m_dbTableName。
    return static_cast<AddItemCountDBScript*>(m_script1c)->m_dbTableName;
}