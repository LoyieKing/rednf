void CUser::set_fatigue_count_db(int type, int value)
{
    // set_fatigue_count_db.md @0x868d4a2：int 实参低字节分别写
    // m_serverFatigueDbType(+0x8eb4d ← type) 与 m_serverFatigueValue(+0x8eb4c ← value)。
    m_serverFatigueDbType = static_cast<char>(type);
    m_serverFatigueValue = static_cast<char>(value);
}