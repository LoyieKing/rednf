void STTowerDialog::clear()
{
    // ORIG W 0x088f2d38：m_index=0、m_name.clear()、三个 flag 字节清零。
    m_index = 0;
    m_name.clear();
    m_flag8 = 0;
    m_flag9 = 0;
    m_flagA = 0;
