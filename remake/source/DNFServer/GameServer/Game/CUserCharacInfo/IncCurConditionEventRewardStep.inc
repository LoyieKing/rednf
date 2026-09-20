void CUserCharacInfo::IncCurConditionEventRewardStep()
{
    if (m_selected)
    {
        ++m_selected->m_field1234;
        if ((short)m_selected->m_field1234 > 0xff)
            m_selected->m_field1234 = 0xff;
    }
}