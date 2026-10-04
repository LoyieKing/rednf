void CUserCharacInfo::SetCurConditionEventRewardStep(short step)
{
    if (m_selected)
    {
    m_selected->m_conditionEventRewardStep = (unsigned short)step;
    }
}