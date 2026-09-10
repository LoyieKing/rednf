void CHARAC_LOAD_MERCENARY::Reset()
{
    m_characNo = 0;
    m_level = 0;
    memset(m_name, 0, sizeof(m_name));                // 0x1e
    memset(m_avatarItems, 0, sizeof(m_avatarItems));  // 0x20
    m_field24 = 0;
    m_field28 = 0;
    m_field2c = 0;
    m_field30 = 0xff;
    m_field31 = 0xff;
    m_avatarBonus = 0xa;
