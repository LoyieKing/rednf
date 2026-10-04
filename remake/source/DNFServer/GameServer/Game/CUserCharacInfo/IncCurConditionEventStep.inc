void CUserCharacInfo::IncCurConditionEventStep()
{
    if (m_selected)
    {
        ++m_selected->m_conditionEventStep;
        if ((short)m_selected->m_conditionEventStep > 0xff)
            m_selected->m_conditionEventStep = 0xff;
    }
}