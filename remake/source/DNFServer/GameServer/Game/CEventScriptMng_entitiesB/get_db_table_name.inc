std::string& CFatigueQuantity::get_db_table_name()
{
    // ORIG 0x0810f988：return &m_dbTableName（FatigueQuantityScript +0x08）。
    return static_cast<FatigueQuantityScript*>(m_script1c)->m_dbTableName;
}