void CUserCharacInfo::SetCurConditionEventRewardStep(short step)
{
    if (m_selected)
    {
    CUR->m_curConditionEventRewardStep = (unsigned short)step;
    }
