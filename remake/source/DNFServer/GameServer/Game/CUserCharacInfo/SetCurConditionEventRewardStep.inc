void CUserCharacInfo::SetCurConditionEventRewardStep(short step)
{
    if (m_selected)
    {
    m_selected->m_field1234 = (unsigned short)step;
    }
}