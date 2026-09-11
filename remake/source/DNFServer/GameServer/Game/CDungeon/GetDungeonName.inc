const char* CDungeon::GetDungeonName() const
{
    // ORIG 0x081455a6：return m_name.c_str()（m_name +0x18）
    return m_name.c_str();
}