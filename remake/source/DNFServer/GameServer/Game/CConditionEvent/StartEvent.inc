void CConditionEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0xd, 0);
    tr("[ConditionEvent] Start!");
