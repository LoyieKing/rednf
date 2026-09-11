// ORIG T 0x811056e（vtable slot 2）：基类 is_eventing 且
// GetRepeatEvent(0x65) 非空且其 IsEventing(0) 为真才返回 true。
bool CUsedFatigueGiveItem::is_eventing()
{
    if (CEventEntity::is_eventing())
    {
        if (GlobalData::s_event_manager->GetRepeatEvent(0x65)->IsEventing(0))
            return true;
    }
    return false;
}