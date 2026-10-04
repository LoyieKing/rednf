void CUserCharacInfo::IncCurConditionEventRewardStep()
{
    if (m_selected)
    {
        ++m_selected->m_conditionEventRewardStep;
        if ((short)m_selected->m_conditionEventRewardStep > 0xff)
            m_selected->m_conditionEventRewardStep = 0xff;
    }
}