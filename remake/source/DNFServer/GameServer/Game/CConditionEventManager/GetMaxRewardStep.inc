int CConditionEventManager::GetMaxRewardStep(short step)
{
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x71)->IsEventing((CUser*)0))
        return 0;
    int field0 = G_CDataManager()->m_conditionEvent.m_field0;
    if ((int)step != field0)
        return 0;
    return G_CDataManager()->m_conditionEvent.m_map8.size();
