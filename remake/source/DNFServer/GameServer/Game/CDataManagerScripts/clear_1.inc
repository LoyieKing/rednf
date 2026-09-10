void GuildParameterScript::clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldC = 0;
    m_map10.clear();
    m_field28 = 0;
    m_vec2c.clear();
    m_vec38.clear();
    m_vecE4.clear();
    m_vecF0.clear();
    m_fieldFC = 0.0;
    m_field104 = 0;
    m_field108 = 0;
    m_field10c = 0;
    m_field110 = 0.0;
    memset(m_levelUpCost, 0, sizeof(m_levelUpCost));  // ORIG：+0x5c memset 0x88
