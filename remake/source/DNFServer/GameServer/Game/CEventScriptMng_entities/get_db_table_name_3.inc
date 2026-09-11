std::string& CPcRoomPlayTime::get_db_table_name()
{
    // ORIG 0x8110864：return &m_dbTableName。
    return static_cast<PcBangPlayTimeScript*>(m_script1c)->m_dbTableName;
}