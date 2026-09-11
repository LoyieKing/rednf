void CUserCharacInfo::IncCurConditionEventStep()
{
    if (m_selected)
    {
        ++CUR->m_curConditionEventStep;
        if ((short)CUR->m_curConditionEventStep > 0xff)
            CUR->m_curConditionEventStep = 0xff;
    }
}