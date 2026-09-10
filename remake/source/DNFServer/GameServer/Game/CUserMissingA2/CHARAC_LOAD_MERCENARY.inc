CHARAC_LOAD_MERCENARY::CHARAC_LOAD_MERCENARY()
{
    m_characNo = 0;
    m_level = 0;
    m_field24 = 0;
    m_field28 = 0;
    m_field2c = 0;
    m_field30 = 0xff;
    m_field31 = 0xff;
    m_avatarBonus = 0xa;
    memset(m_name, 0, 0x1e);
    memset(m_avatarItems, 0, 0x20);
