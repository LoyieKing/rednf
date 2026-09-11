std::string& CLevelUpReward::get_db_table_name()
{
    // ORIG 0x810e2d6：return &m_dbTableName（调用方按 std::string& 解释）。
    return static_cast<LevelUpRewardScript*>(m_script1c)->m_dbTableName;
}