// ORIG 0x0810f9c0：return &m_dbTableName（PurchaseCashItemBonusScript +0x08）。
std::string& CPurchaseCashItemBonus::get_db_table_name()
{
    return static_cast<PurchaseCashItemBonusScript*>(m_script1c)->m_dbTableName;
}