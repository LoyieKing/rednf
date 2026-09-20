void CUserCharacInfo::SetCurConditionEventStep(short step)
{
    if (m_selected)
    {
    m_selected->m_field1232 = (unsigned short)step;
    }
}