bool CUsedFatigueGiveItem::is_eventing()
{
    if (CEventEntity::is_eventing())
    {
        if (GlobalData::s_event_manager->GetRepeatEvent(0x65)->IsEventing(0))
            return true;
    }
    return false;
