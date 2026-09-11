std::string& CAccountFirstLogin::get_db_table_name()
{
    // ORIG 0x810debe：return &m_dbTableName。
    return static_cast<AccountFirstLoginScript*>(m_script1c)->m_dbTableName;
}