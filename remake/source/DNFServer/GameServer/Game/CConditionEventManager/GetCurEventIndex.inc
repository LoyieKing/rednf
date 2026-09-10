int CConditionEventManager::GetCurEventIndex()
{
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x71)->IsEventing((CUser*)0))
        return 0;
    return G_CDataManager()->m_conditionEvent.m_field0;
