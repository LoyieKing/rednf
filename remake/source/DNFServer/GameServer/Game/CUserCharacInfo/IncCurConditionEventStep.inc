void CUserCharacInfo::IncCurConditionEventStep()
{
    if (m_selected)
    {
        ++m_selected->m_field1232;
        if ((short)m_selected->m_field1232 > 0xff)
            m_selected->m_field1232 = 0xff;
    }
}