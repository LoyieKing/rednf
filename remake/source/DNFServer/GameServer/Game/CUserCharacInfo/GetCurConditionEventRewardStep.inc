unsigned short CUserCharacInfo::GetCurConditionEventRewardStep() const
{
    return m_selected ? m_selected->m_conditionEventRewardStep : 0;
}