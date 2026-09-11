// ORIG W 0x0844d584 _ZN21STBreakAwayMemberData5clearEv。
void STBreakAwayMemberData::clear()
{
    m_field00 = 0;
    memset(m_name, 0, 0x1f);
    m_field24 = 0;
    m_field28 = 0;
    m_field2c = 0;
    m_field30 = 0;
    m_field34 = 0;
    m_field38 = 0;
    m_field3c = 0;
    m_field40 = 0;
}
