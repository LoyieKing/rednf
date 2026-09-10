void CUserCharacInfo::SetCurConditionEventStep(short step)
{
    if (m_selected)
    {
    CUR->m_curConditionEventStep = (unsigned short)step;
    }
