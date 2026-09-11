unsigned short CUserCharacInfo::GetCurConditionEventStep() const
{
    return m_selected ? CUR->m_curConditionEventStep : 0;
}