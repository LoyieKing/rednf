std::string& CUsedFatigueGiveItem::get_db_table_name()
{
    // ORIG 0x81105e8：return &m_dbTableName。
    return static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_dbTableName;
}