void CUserCharacInfo::SetCurConditionEventStep(short step)
{
    if (m_selected)
    {
    m_selected->m_conditionEventStep = (unsigned short)step;
    }
}