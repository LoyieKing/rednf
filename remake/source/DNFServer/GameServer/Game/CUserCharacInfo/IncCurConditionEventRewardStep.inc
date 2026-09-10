void CUserCharacInfo::IncCurConditionEventRewardStep()
{
    if (m_selected)
    {
        ++CUR->m_curConditionEventRewardStep;
        if ((short)CUR->m_curConditionEventRewardStep > 0xff)
            CUR->m_curConditionEventRewardStep = 0xff;
    }
